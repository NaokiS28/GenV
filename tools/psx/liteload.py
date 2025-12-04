#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, socket, time
from binascii import crc32
from argparse import ArgumentParser, FileType
from serial   import Serial

# polyfill for TCP serial
class TCPSerial:
	def __init__(self, host, port):
		self.socket = socket.create_connection(( host, port ), 1)

	def __enter__(self):
		return self

	def __exit__(self, *err):
		self.socket.close()

	def read(self, length):
		try:
			return self.socket.recv(length)
		except socket.timeout:
			return b""

	def write(self, data):
		self.socket.sendall(data)

	def flush(self):
		pass

def send_packet(dev, data):
	for attempt in range(1, 4):
		if data:
			dev.write(data)
			time.sleep(0.05)

		resp = dev.read(1)

		if resp == (b"K" if data else b"R"):
			return
		elif resp == b"E": # fatal error
			raise RuntimeError("liteload reported an error")
		elif resp == b"C": # CRC error
			print(f"Payload CRC mismatch, retrying ({attempt})", file = sys.stderr)
		elif not resp:
			print(f"No response received, retrying ({attempt})", file = sys.stderr)

	raise RuntimeError("too many failed attempts")

def main():
	parser = ArgumentParser()
	parser.add_argument(
		"-t", "--tcp",
		type    = int,
		help    = "use TCP connection instead of serial",
		metavar = "port"
	)
	parser.add_argument(
		"-c", "--chunk",
		type    = int,
		default = 1024,
		help    = "transfer chunk length (default 1024)",
		metavar = "len"
	)
	parser.add_argument(
		"-s", "--skip-ready",
		action = "store_true",
		help   = "do not wait for ready message"
	)
	parser.add_argument(
		"-b", "--set-bpc",
		action = "store_true",
		help   = "set $bpc register (cop0r3) to match $pc"
	)
	parser.add_argument(
		"-f", "--force-sp",
		action = "store_true",
		help   = "force $sp register to be set"
	)
	parser.add_argument(
		"device",
		help = "serial port (or hostname when using -t)"
	)
	parser.add_argument(
		"file",
		type    = FileType("rb"),
		help    = "PS1 executable to load",
		metavar = "file"
	)
	args = parser.parse_args()

	# load exe
	with args.file as exe:
		header = exe.read(2048)
		if header[0:8] != b"PS-X EXE":
			parser.error("file is not a valid executable")

		header = header[0x10:0x4c] # struct EXEC
		length = int.from_bytes(header[0x0c:0x10], "little") # .t_size
		data   = exe.read(length)

	flags = int(args.set_bpc) | (int(args.force_sp) << 1)

	try:
		# open port
		if args.tcp:
			dev = TCPSerial(args.device, args.tcp)
		else:
			dev = Serial(args.device, 115200, timeout = 1)
			dev.reset_input_buffer()

		# send exe
		with dev:
			if not args.skip_ready:
				print("Waiting for ping...", end = "\r", file = sys.stderr)
				send_packet(dev, None)

			print("Sending commands...", end = "\r", file = sys.stderr)
			send_packet(dev, b"MEXE")

			print("Sending executable header...", end = "\r", file = sys.stderr)
			meta  = flags.to_bytes(4, "little")
			meta += crc32(data).to_bytes(4, "little")
			send_packet(dev, meta + header)

			for offset in range(0, length, args.chunk):
				chunk    = data[offset:offset + args.chunk]
				progress = int(offset / length * 100)

				meta  = len(chunk).to_bytes(4, "little")
				meta += crc32(chunk).to_bytes(4, "little")

				print(f"Sending chunk: 0x{offset:08x} ({progress:2d}%)", end = "\r", file = sys.stderr)
				send_packet(dev, meta)
				send_packet(dev, chunk)

	except BaseException as err:
		parser.error(err)

	print("\nExecutable loaded.", file = sys.stderr)
	sys.exit(0)

if __name__ == "__main__":
	main()
