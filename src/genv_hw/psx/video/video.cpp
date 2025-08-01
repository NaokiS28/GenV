/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu-d3d.hpp - Created on 24-04-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "video.hpp"
#include "../registers.h"

#define GPUC (this->*GPUCMD)
#define GP0RDY(cmdcount)                              \
	if (!useDMA)                                      \
	{                                                 \
		waitForGP0Ready();                            \
	}                                                 \
	else                                              \
	{                                                 \
		gpuListPtr = allocatePacket(chain, cmdcount); \
	}

PSXGPU::PSXGPU()
{
}

void PSXGPU::directWrite(uint32_t cmd)
{
	GPU_GP0 = cmd;
}

void PSXGPU::addToDMAList(uint32_t cmd)
{
	gpuListPtr[dmaPtrIdx++] = cmd;
}

bool PSXGPU::init()
{
	gpuMode = static_cast<GP1VideoMode>(GPU_GP1 & GP1_STAT_FB_MODE_BITMASK);
	GPU_GP1 = gp1_resetGPU();
	setResolution(640, 480);

	GP0RDY(4);
	GPUC(gp0_texpage(0, true, false));
	GPUC(gp0_fbOffset1(0, 0));
	GPUC(gp0_fbOffset2(320 - 1, 240 - 1));
	GPUC(gp0_fbOrigin(0, 0));
	fillScreen(Colors::Blue);
	swapFrameBuffer();
	fillScreen(Colors::Blue);
	swapFrameBuffer();
	GPU_GP1 = gp1_fbOffset(0, 0);
	GPU_GP1 = gp1_dispBlank(false);
	// enableDMA(true);
	return 0;
}

int PSXGPU::setResolution(int w, int h, bool updateWindow)
{
	// Set the origin of the displayed framebuffer. These "magic" values,
	// derived from the GPU's internal clocks, will center the picture on most
	// displays and upscalers.
	int x = 0x760;
	int y = (gpuMode == GP1_MODE_PAL) ? 0xa3 : 0x88;

	int result = 0;
	if (w > 640 || h > 320)
		result = V_RES_TOO_HIGH;
	w = (w > 640 ? 640 : w);
	h = (h > 480 ? 480 : h);

	uint32_t mode = findNearestVideoMode(&PSX_Video_Modes, w, h);
	if (mode < 0)
	{
		LOG_GPU("findNearestVideoMode(%u,%u,%u) failed with %u.", &PSX_Video_Modes, w, h, mode);
		return mode;
	}

	VideoResolution vidMode = PSX_Video_Modes.resList[(mode & 0x7F)];
	if (mode & V_RES_MODIFIED)
	{
		LOG_GPU("requested %ux%u, but this is not a valid mode. Using %ux%u instead.", w, h, vidMode.width, vidMode.height);
		if (!result)
			result = V_RES_MODIFIED;
	}

	screen.res.width = vidMode.width;
	screen.res.height = vidMode.height;
	screen.refreshRate = (mode == GP1_MODE_NTSC ? 60 : 50); // Refresh rate is constant.

	// Set the resolution. The GPU provides a number of fixed horizontal (256,
	// 320, 368, 512, 640) and vertical (240-256, 480-512) resolutions to pick
	// from, which affect how fast pixels are output and thus how "stretched"
	// the framebuffer will appear.
	GP1HorizontalRes horizontalRes = GP1HorizontalResList[mode % 5];
	GP1VerticalRes verticalRes = GP1VerticalResList[((mode & 0x7F) >= 10)]; // Either 256 or 512, no inbetween

	// Set the number of displayed rows and columns. These values are in GPU
	// clock units rather than pixels, thus they are dependent on the selected
	// resolution.
	int offsetX = (w * gp1_clockMultiplierH(horizontalRes)) / 2;
	int offsetY = (h / gp1_clockDividerV(verticalRes)) / 2;

	bool interlace = false;
	if (verticalRes != GP1_VRES_256)
		interlace = true;

	// Hand all parameters over to the GPU by sending GP1 commands.
	GP0RDY(3);
	GPU_GP1 = gp1_fbRangeH(x - offsetX, x + offsetX);
	GPU_GP1 = gp1_fbRangeV(y - offsetY, y + offsetY);
	GPU_GP1 = gp1_fbMode(
		horizontalRes,
		verticalRes,
		gpuMode,
		interlace,
		GP1_COLOR_16BPP);

	return result;
}

void PSXGPU::waitForGP0Ready(void)
{
	// Block until the GPU reports to be ready to accept commands through its
	// status register (which has the same address as GP1 but is read-only).
	while (!(GPU_GP1 & GP1_STAT_CMD_READY))
		__asm__ volatile("");
}

void PSXGPU::waitForDMADone(void) {
	while (DMA_CHCR(DMA_GPU) & DMA_CHCR_ENABLE)
		__asm__ volatile("");
}

void PSXGPU::fillScreen(Color color)
{
	GP0RDY(3);
	GPUC(gp0_rgb(color.r, color.g, color.b) | gp0_vramFill());
	GPUC(gp0_xy(frameX, frameY));
	GPUC(gp0_xy(screen.res.width, screen.res.height));
}

void PSXGPU::waitForVSync(void)
{
	// The GPU won't tell us directly whenever it is done sending a frame to the
	// display, but it will send a signal to another peripheral known as the
	// interrupt controller (which will be covered in a future tutorial). We can
	// thus wait until the interrupt controller's vertical blank flag gets set,
	// then reset (acknowledge) it so that it can be set again by the GPU.
	while (!(IRQ_STAT & (1 << IRQ_VSYNC)))
		__asm__ volatile("");

	IRQ_STAT = ~(1 << IRQ_VSYNC);
}

void PSXGPU::swapFrameBuffer()
{
	if (useDMA)
	{
		chain = &dmaChains[screenBufferPage];
		chain->nextPacket = chain->data;
	}

	if (useDoubleBuffer)
		screenBufferPage = !screenBufferPage;
	else
		screenBufferPage = 0;

	frameX = (screenBufferPage ? screen.res.width : 0);
	frameY = 0;

	GPU_GP1 = gp1_fbOffset(frameX, frameY);

	GP0RDY(4);
	GPUC(gp0_texpage(0, true, false));
	GPUC(gp0_fbOffset1(frameX, frameY));
	GPUC(gp0_fbOffset2(
		frameX + screen.res.width - 1,
		frameY + screen.res.height - 2));
	GPUC(gp0_fbOrigin(frameX, frameY));
}

uint32_t *PSXGPU::allocatePacket(DMAChain *chain, int numCommands)
{
	// Grab the current pointer to the next packet then increment it to allocate
	// a new packet. We have to allocate an extra word for the packet's header,
	// which will contain the number of GP0 commands the packet is made up of as
	// well as a pointer to the next packet (or a special "terminator" value to
	// tell the DMA unit to stop).
	uint32_t *ptr = chain->nextPacket;
	chain->nextPacket += numCommands + 1;

	// Write the header and set its pointer to point to the next packet that
	// will be allocated in the buffer.
	*ptr = gp0_tag(numCommands, chain->nextPacket);

	// Make sure we haven't yet run out of space for future packets or a linked
	// list terminator, then return a pointer to the packet's first GP0 command.
	assert(chain->nextPacket < &(chain->data)[iPSXDMAListSize]);
	dmaPtrIdx = 0;
	return &ptr[1];
}

void PSXGPU::sendLinkedList(const void *data)
{
	// Wait until the GPU's DMA unit has finished sending data and is ready.
	while (DMA_CHCR(DMA_GPU) & DMA_CHCR_ENABLE)
		__asm__ volatile("");

	// Make sure the pointer is aligned to 32 bits (4 bytes). The DMA engine is
	// not capable of reading unaligned data.
	assert(!((uint32_t)data % 4));

	// Give DMA a pointer to the beginning of the data and tell it to send it in
	// linked list mode. The DMA unit will start parsing a chain of "packets"
	// from RAM, with each packet being made up of a 32-bit header followed by
	// zero or more 32-bit commands to be sent to the GP0 register.
	DMA_MADR(DMA_GPU) = (uint32_t)data;
	DMA_CHCR(DMA_GPU) = 0 | DMA_CHCR_WRITE | DMA_CHCR_MODE_LIST | DMA_CHCR_ENABLE;
}

bool PSXGPU::beginRender()
{
	swapFrameBuffer();
	return 1;
}

bool PSXGPU::endRender()
{
	if (useDMA)
		*(chain->nextPacket) = gp0_endTag(0);
	waitForGP0Ready();
	waitForVSync();
	if (useDMA)
		sendLinkedList(chain->data);
	return 1;
}

void PSXGPU::enableDMA(bool state)
{
	if (state)
	{
		useDMA = true;
		GPUCMD = &PSXGPU::addToDMAList;
		DMA_DPCR |= DMA_DPCR_CH_ENABLE(DMA_GPU);
		GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);
	}
	else
	{
		useDMA = false;
		GPUCMD = &PSXGPU::directWrite;
		DMA_DPCR = (DMA_DPCR & ~DMA_DPCR_CH_ENABLE(DMA_GPU));
		GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_NONE);
	}
}

int PSXGPU::uploadTexture(Textures::TextureObject *tObj){

	uploadTexture();
}

int PSXGPU::releaseTexture(Textures::TextureObject *tObj){

}


int PSXGPU::uploadTexture(
	Textures::TextureObject *tObj,
	int x,
	int y
)
{
	waitForDMADone();
	assert(!((uint32_t) tObj->bitmap % 4));

	// Calculate how many 32-bit words will be sent from the width and height of
	// the texture. If more than 16 words have to be sent, configure DMA to
	// split the transfer into 16-word chunks in order to make sure the GPU will
	// not miss any data.
	size_t length = (tObj->width * tObj->height) / 2;
	size_t chunkSize, numChunks;

	if (length < bPSXDMAChunkSize) {
		chunkSize = length;
		numChunks = 1;
	} else {
		chunkSize = bPSXDMAChunkSize;
		numChunks = length / bPSXDMAChunkSize;

		// Make sure the length is an exact multiple of 16 words, as otherwise
		// the last chunk would be dropped (the DMA unit does not support
		// "incomplete" chunks). Note that this will impose limitations on the
		// size of VRAM uploads.
		assert(!(length % bPSXDMAChunkSize));
	}

	// Put the GPU into VRAM upload mode by sending the appropriate GP0 command
	// and our coordinates.
	waitForGP0Ready();
	GPU_GP0 = gp0_vramWrite();
	GPU_GP0 = gp0_xy(x, y);
	GPU_GP0 = gp0_xy(tObj->width, tObj->height);

	// Give DMA a pointer to the beginning of the data and tell it to send it in
	// slice (chunked) mode.
	DMA_MADR(DMA_GPU) = (uint32_t) tObj->bitmap;
	DMA_BCR (DMA_GPU) = chunkSize | (numChunks << 16);
	DMA_CHCR(DMA_GPU) = 0
		| DMA_CHCR_WRITE
		| DMA_CHCR_MODE_SLICE
		| DMA_CHCR_ENABLE;
}