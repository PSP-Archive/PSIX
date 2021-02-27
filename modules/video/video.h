#define IN_BUFFER_SIZE 1024
#define OUT_BUFFER_SIZE 512

typedef struct videoFile
{
	int fd;
	char fileName;
	int currentFrame;
	unsigned char inBuffer[IN_BUFFER_SIZE];
	unsigned char outBuffer[OUT_BUFFER_SIZE];
	
} videoFile;
