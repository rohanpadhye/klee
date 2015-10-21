#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <klee/Internal/ADT/KTest.h>  

// Constant globals for the model_version
static char modelVersionName[] = "model_version";
static unsigned int modelVersionNumber = 1;

// Globally allocated KTest object
static KTest kTest;

int main(int argc, char** argv) {
  using namespace std;
  // Initialize kTest with default version, 0 args and 0 objects
  kTest.version = modelVersionNumber;
  kTest.numArgs = 0;
  kTest.args = NULL;
  kTest.numObjects = 0;
  kTest.objects = NULL;

  // Allocate a vector of args (Note: The underlying array will be used later)
  vector<char* > args;

  // argsMode is a flag that is true until we encounter a '-' argument
  bool argsMode = true;

  // Offset will be used later to compute the index after the '-' arg
  int offset;

  // Loop over args
  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];
    // Processing depends on argsMode
    if (argsMode) {
      // If we see a '-' we switch from argsMode to objectsMode
      if (strcmp(arg, "-") == 0) {
        argsMode = false;
        kTest.numObjects = argc-i; // 1 extra object is for model_version
        kTest.objects = (KTestObject*) malloc(sizeof(KTestObject)*kTest.numObjects);
        offset = i; 
      } else {
        kTest.numArgs++;
        args.push_back(arg);
      }
    } else {
      // In objectsMode, we initialize one object at a time indexed with offset
      int o = i - offset;
      KTestObject* obj =  &kTest.objects[o];

      // Find the index of the '=' character in the arg
      int eqIdx = -1;
      char* valueStart = NULL;
      int j = 0;
      char c;
      do {
        c = arg[j];
        if (c == '=') {
          eqIdx = j;
          valueStart = &arg[j+1];
          break;
        }
        j++;
      } while (c != '\0');

      // Ensure that the '=' exists
      if (eqIdx != -1) {
        // Create a new string from the start of arg till '=' as the name
        obj->name = (char*) malloc(sizeof(char) * (eqIdx+1));
        strncpy(obj->name, arg, eqIdx);
        obj->name[eqIdx] = '\0';
        // Create a new string from the '=' till the end of arg as the value
        obj->numBytes = strlen(arg) - eqIdx;
        obj->bytes = (unsigned char*) malloc(sizeof(unsigned char) * obj->numBytes);
        strncpy((char*) obj->bytes, valueStart, obj->numBytes);
      } else {
        // As a fall back, have a name and no value
        obj->name = arg;
        obj->numBytes = 0;
        obj->bytes = NULL;
      }
    }
  }

  if (argsMode) {
    kTest.objects = (KTestObject*) malloc(sizeof(KTestObject));
  }

  KTestObject* obj = &kTest.objects[0];
  obj->name = modelVersionName;
  obj->numBytes = sizeof(modelVersionNumber);
  obj->bytes = (unsigned char*) &modelVersionNumber;

  
  kTest.args = args.data();
  kTest.symArgvs = 0;
  kTest.symArgvLen = 0;

  kTest_toFile(&kTest, "/dev/stdout");


  return 0;
}
