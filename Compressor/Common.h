#define MAX_CHARS_IN_LINE 200
#define DEBUG_ALLOWED 1

#define MAX_BUFFER_BITS 32

#define VERTEX_FILE "verts.txt"
#define BITSTREAM_FILE "bitStream.dat"
#define DEBUG_FILE	"compDebug.txt"

unsigned int buffer32;
int remainingBufferBits;

//Data structures
typedef struct LinkedList{

	struct LinkedList *next;
	struct LinkedList *prev;

}LinkedList;

typedef struct CommonDataStr
{
	double minX;
	double minY;
	double minZ;

	double maxX;
	double maxY;
	double maxZ;

	double bucketSizeX;
	double bucketSizeY;
	double bucketSizeZ;

	int numBits;
	int numVerts;
	
	LinkedList vertDataHead;
}CommonDataStr;

typedef struct VertexDataStr
{
	LinkedList listHandle;

	double vertX;
	double vertY;
	double vertZ;
}VertexDataStr;

//List related function prototypes
void ListInit(LinkedList *head);
void ListInsertTail(LinkedList *head, LinkedList *entry);
LinkedList *ListFirst(LinkedList *head);
LinkedList *ListNext(LinkedList *head, LinkedList *entry);
LinkedList *ListRemoveHead(LinkedList *head);

//Other function prototypes
void ParseVertFile(FILE *vertFilePtr, CommonDataStr *commonDataPtr);
void CompressToBitStream(CommonDataStr *commonDataPtr);
void FreeVertData(CommonDataStr *commonDataPtr);