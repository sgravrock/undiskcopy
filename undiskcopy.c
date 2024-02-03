#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>

// Extracts raw images from Disk Copy 4.2 images.
//
// The image name is ignored, as are tags (I don't know what to do with them, 
// and I don't have any images that use them). That's unlikely to be a problem
// for Mac images but likely means that this program won't work with Lisa
// images.
//
// Format description: https://web.archive.org/web/20201028142058/https://wiki.68kmla.org/DiskCopy_4.2_format_specification#0x40-0x43:_Data_block_size_in_bytes

typedef struct {
	uint8_t name_len;
	char name[63];
	// All 16- and 32-bit integer fields are big-endian.
	uint32_t data_sz;
	uint32_t tag_sz;
	uint32_t data_checksum;
	uint32_t tag_checksum;
	uint8_t disk_encoding;
	uint8_t format;
	uint16_t magic;
} diskcopy42_header;


static FILE *xfopen(const char *path, const char *mode);


int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s input-file output-file\n",
			argv[0] ? argv[0] : "undiskcopy");
		return EXIT_FAILURE;
	}

	FILE *infp = xfopen(argv[1], "rb");
	FILE *outfp = xfopen(argv[2], "wb");
	diskcopy42_header header;

	if (fread(&header, sizeof header, 1, infp) != 1) {
		perror("fread");
		return EXIT_FAILURE;
	}

	uint16_t magic = ntohs(header.magic);

	if (magic != 0x0100) {
		fprintf(stderr, "This doesn't look like a Disk Copy 4.2 file\n"
			"(Expected a magic number of 0x0100 but got 0x%x)\n", magic);
		return EXIT_FAILURE;
	}

	if (header.tag_sz != 0) {
		fprintf(stderr, "Warning: this disk image has tags but they will "
			"be ignored.\n");
		fprintf(stderr, "You can probably ignore this warning unless it's "
			"a Lisa image.\n");
	}

	uint32_t data_sz = ntohl(header.data_sz);

	// Normal images have one of these sizes. Other sizes are potentially
	// valid, but unusual.
	if (data_sz != 409600           // 400k GCR
			&& data_sz != 819200      // 800k GCR
			&& data_sz != 737280      // 720k MFM (DOS, maybe?)
			&& data_sz != 1474560) {  // 1440k MFM
		fprintf(stderr, "Warning: Unusual image size %u\n", data_sz);
	}

	uint32_t checksum = 0;

	for (uint32_t i = 0; i < data_sz / 2; i++) {
		uint16_t word;

		if (fread(&word, sizeof word, 1, infp) != 1) {
			perror("fread");
			return EXIT_FAILURE;
		}

		if (fwrite(&word, sizeof word, 1, outfp) != 1) {
			perror("fwrite");
			return EXIT_FAILURE;
		}

		checksum += ntohs(word);
		// Rotate right
		checksum = ((checksum & 1) << 31) | ((checksum >> 1) & ~(1 << 31));
	}

	fclose(infp);

	if (fclose(outfp) != 0) {
		perror("fclose");
		return EXIT_FAILURE;
	}

	uint32_t expected_checksum = ntohl(header.data_checksum);

	if (checksum != expected_checksum) {
		fprintf(stderr, "Checksum mismatch: expected=%u actual=%u\n",
			expected_checksum, checksum);
		fprintf(stderr, "This could be a problem with the image or a bug in "
			"undiskcopy\n");
		return EXIT_FAILURE;
	}

	return 0;
}

static FILE *xfopen(const char *path, const char *mode) {
	FILE *fp = fopen(path, mode);

	if (!fp) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	return fp;
}
