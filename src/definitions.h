#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ERROR_READ_BLOCK  (-5)
#define BAD_NAME_FILE     (-4)
#define BAD_NUM_OF_PARAMS (-3)
#define UNKNOWN_PARAM     (-2)
#define ALLOCATE_FAIL     (-1)
#define RES_OK            (0)

#define BLOCK_SIZE        (512)
#define BLOCK_HEADER_SIZE (12)
#define TMP_BUFFER        (50)
#define RECORD_MAX_SIZE   (208)