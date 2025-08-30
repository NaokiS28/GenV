#!/usr/bin/env python3

# DONT USE THIS - Is currently broken.

# GenV - Copyright (C) 2025 NaokiS, spicyjpeg
# pyterm.py - Created on 24-08-2025
# 
# GenV is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
# 
# GenV is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# GenV. If not, see <https://www.gnu.org/licenses/>.

"""
genv-term: minimal TCP terminal client with ANSI passthrough
- Defaults: localhost:6699
- Raw mode (POSIX) so arrows, Ctrl+C, etc. go through
- Windows: enables Virtual Terminal (ANSI) modes
"""
import argparse
import os
import socket
import sys
import threading

# ---------- Windows ANSI + raw-ish input helpers ----------
def _win_enable_vt():
    """Enable ANSI escape processing on Windows 10+ consoles."""
    try:
        import msvcrt, ctypes
        kernel32 = ctypes.windll.kernel32
        hOut = kernel32.GetStdHandle(-11)  # STD_OUTPUT_HANDLE
        hIn  = kernel32.GetStdHandle(-10)  # STD_INPUT_HANDLE
        mode_out = ctypes.c_uint()
        mode_in  = ctypes.c_uint()
        kernel32.GetConsoleMode(hOut, ctypes.byref(mode_out))
        kernel32.GetConsoleMode(hIn,  ctypes.byref(mode_in))
        ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004
        DISABLE_NEWLINE_AUTO_RETURN = 0x0008
        ENABLE_VIRTUAL_TERMINAL_INPUT = 0x0200
        kernel32.SetConsoleMode(hOut, mode_out.value | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN)
        kernel32.SetConsoleMode(hIn,  mode_in.value  | ENABLE_VIRTUAL_TERMINAL_INPUT)
        return True
    except Exception:
        return False

def _win_input_loop(sock):
    """Read keypresses with msvcrt (non-blocking) and send bytes."""
    import msvcrt
    try:
        while True:
            if msvcrt.kbhit():
                ch = msvcrt.getwch()
                # Convert Python str (UTF-16 code unit) into UTF-8 bytes
                sock.sendall(ch.encode('utf-8'))
            # Yield a tiny bit to avoid busy loop
            import time; time.sleep(0.001)
    except Exception:
        pass

# ---------- POSIX raw mode helpers ----------
class _PosixTTY:
    def __init__(self):
        self._orig = None
    def __enter__(self):
        import termios, tty
        self.fd = sys.stdin.fileno()
        self._orig = termios.tcgetattr(self.fd)
        tty.setraw(self.fd)  # raw: pass through everything
        return self
    def __exit__(self, exc_type, exc, tb):
        if self._orig:
            import termios
            termios.tcsetattr(self.fd, termios.TCSADRAIN, self._orig)

def _posix_input_loop(sock):
    """Read raw bytes from stdin and forward to socket."""
    import select
    try:
        while True:
            r, _, _ = select.select([sys.stdin], [], [], 0.05)
            if sys.stdin in r:
                data = os.read(sys.stdin.fileno(), 4096)
                if not data:
                    break
                sock.sendall(data)
    except Exception:
        pass

# ---------- Receiver ----------
def recv_loop(sock):
    """Receive from socket and write to stdout (ANSI passthrough)."""
    try:
        while True:
            data = sock.recv(4096)
            if not data:
                # Remote closed
                sys.stdout.write("\r\n[remote closed]\r\n")
                sys.stdout.flush()
                break
            sys.stdout.buffer.write(data)
            sys.stdout.flush()
    except Exception:
        pass

def main():
    p = argparse.ArgumentParser(description="Minimal TCP terminal client with ANSI passthrough")
    p.add_argument("-url", default="localhost", help="Host (default: localhost)")
    p.add_argument("-port", type=int, default=6699, help="Port (default: 6699)")
    args = p.parse_args()

    # Connect
    try:
        sock = socket.create_connection((args.url, args.port))
    except Exception as e:
        print(f"Failed to connect to {args.url}:{args.port} -> {e}")
        return
    print(f"[connected to {args.url}:{args.port}]\r")

    # Start receiver
    t_recv = threading.Thread(target=recv_loop, args=(sock,), daemon=True)
    t_recv.start()

    # Platform-specific input mode
    if os.name == "nt":
        _win_enable_vt()
        try:
            _win_input_loop(sock)
        except KeyboardInterrupt:
            pass
        finally:
            try: sock.shutdown(socket.SHUT_WR)
            except Exception: pass
            sock.close()
    else:
        try:
            with _PosixTTY():
                _posix_input_loop(sock)
        except KeyboardInterrupt:
            pass
        finally:
            try: sock.shutdown(socket.SHUT_WR)
            except Exception: pass
            sock.close()
            # Restore a sane line after raw mode
            sys.stdout.write("\r\n")
            sys.stdout.flush()

if __name__ == "__main__":
    main()
