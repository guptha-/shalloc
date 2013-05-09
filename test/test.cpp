/*==============================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  This executes the tests and generates results
 *
 * =============================================================================
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>

using namespace std;

#define MAX_TESTCASES 6
/* ===  FUNCTION  ==============================================================
 *         Name:  main
 * =============================================================================
 */
int main (int argc, char *argv[])
{
  char tool[10];
  if (argc == 2 && 
      (strcmp(argv[1], "--verbose") == 0 || (strcmp(argv[1], "-v") == 0))) {

    strcpy(tool, "diff");
  } else {
    strcpy(tool, "cmp -s");
  }

  for (int i = 1; i <= MAX_TESTCASES; i++) {
    usleep(10000);
    char command[100];
    sprintf(command, "bin/test%d > test/recent/temp%d", i, i);
    system(command);
    usleep(10000);
    sprintf(command, "grep -v IGNORE test/recent/temp%d >test/recent/temp%d_final", i , i);
    system(command);
    usleep(10000);
    sprintf(command, 
            "%s test/output/temp%d_final test/recent/temp%d_final", tool, i, i);
    if (system(command) == 0){
      cout<<"Test case "<<i<<" has passed"<<endl;
    } else {
      cout<<"Test case "<<i<<" has FAILED"<<endl;
    }
  }
  return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
