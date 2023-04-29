#include <stdio.h>
#include <fstream>
#include "hb-subset.h"

enum RESP_CODE
{
	SUCCESS = 0,
	FILE_NOTFOUND = -1
};

#pragma pack(1)
typedef struct
{
	const char *data;
	int size;
} ResultInfo;

extern "C" uint8_t generateSubset(const char *inputFile, int *codePoints, int codePointSize, ResultInfo &result)
{
	printf("Input File Name:%s\n", inputFile);
	hb_blob_t *origBlob = hb_blob_create_from_file_or_fail(inputFile);
	if (origBlob == NULL)
	{
		printf("Input File Not Found\n");
		return RESP_CODE::FILE_NOTFOUND;
	}
	hb_face_t *origFace = hb_face_create(origBlob, 0);

	hb_subset_input_t *input = hb_subset_input_create_or_fail();
	hb_set_t *unicodes = hb_subset_input_unicode_set(input);
	hb_set_clear(unicodes);

	printf("CodePoints:[total = %d][", codePointSize);
	for (uint32_t i = 0; i < codePointSize; i++)
	{
		if (i != 0)
			printf(" ");
		printf("%d", codePoints[i]);
		hb_set_add(unicodes, codePoints[i]);
	}
	printf("]\n");

	hb_face_t *newFace = hb_subset_or_fail(origFace, input);
	hb_blob_t *newBlob = hb_face_reference_blob(newFace);

	uint32_t *resultSize = new uint32_t;

	const char *tmp = hb_blob_get_data(newBlob, resultSize);
	const char *resultData = new char[*resultSize];
	memmove((void *)resultData, tmp, *resultSize);

	result.data = resultData;
	result.size = *resultSize;

	hb_subset_input_destroy(input);

	hb_blob_destroy(origBlob);
	hb_blob_destroy(newBlob);

	hb_face_destroy(origFace);
	hb_face_destroy(newFace);

	return RESP_CODE::SUCCESS;
}
