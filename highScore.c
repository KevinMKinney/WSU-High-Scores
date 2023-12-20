#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for making 1mil data
#include <unistd.h>
#include <sys/wait.h>

#define playerSize 1000000
#define skills 6
#define countingArraySize 10000
#define ratioOfLargeValues 0.05 // real ratio is about .02 
#define print 1

// I want to give an argument to the compare function.
// "qsort_r" allows me to do that, but it's implicitly
// defined without this header.
void qsort_r(void *base, size_t nmemb, size_t size,
	int (*compar)(const void *, const void *, void *),
	void *arg);

// code to make 1mil sample data (could be randomized better)
void randNums() {
	FILE* fp = fopen("big_sample.dat", "w");

	srand(time(0));
	int num;
	if(fork()) {
		wait(NULL);
		for (int i = 0; i < 980000; i++) {
			for (int j = 0; j < 5; j++) {
				num = (rand() % 2000);
				fprintf(fp, "%d ", num);
			}
			fprintf(fp, "\n");

		}
	} else {
		for (int i = 0; i < 20000; i++) {
			for (int j = 0; j < 5; j++) {
				num = (rand() % (5000000 - 10000 + 1)) + 10000;
				fprintf(fp, "%d ", num);
			}
			fprintf(fp, "\n");
		}
	}

	fclose(fp);
	exit(0);
}

// read stdin into data array
void readInput(int* arr) {
	for (int i = 0; i < playerSize; i++) {
		for (int j = 0; j < (skills-1); j++) {
			scanf("%d", &arr[i * skills + j]);
			arr[i * skills + (skills-1)] += arr[i * skills + j];
		}
	}
}

// compare function for qsort_r
int cmp(const void* a, const void* b, void* index) {
	return (((int*)b)[*(int*)index] - ((int*)a)[*(int*)index]);
}

// heap sort code:
void swap(int* heap, int a, int b) {
	int temp = heap[a];
	heap[a] = heap[b];
	heap[b] = temp;
}

void heapUp(int* heap, int index) {
	int parentIndex = index >> 1; //(int)(index/2)
	while((index != 0) && (heap[index] > heap[parentIndex])) {
		swap(heap, index, parentIndex);
		index = parentIndex;
		parentIndex = index >> 1;
	}
}

void heapDown(int* heap, int index, int heapSize) {
	int left = (index << 1)+1;
	int right = left + 1;
	while ((right < heapSize) || (left < heapSize)) {
		if ((heap[right] > heap[left]) && (heap[right] > heap[index])) {
			swap(heap, index, right);

			index = right;
			left = (index << 1)+1;
			right = left + 1;
		} else {
			if (heap[left] > heap[index]) {
				swap(heap, index, left);

				index = left;
				left = (index << 1)+1;
				right = left + 1;
			} else {
				return;
			}
		}

	}
} 

int main(int argc, char* argv[]) {

	// var init
	int* skillData = calloc((playerSize * skills), sizeof(int));
	char* skillType[skills] = { "SKILL_BREAKDANCING",
		"SKILL_APICULTURE",
		"SKILL_BASKET",
		"SKILL_XBASKET",
		"SKILL_SWORD",
		"TOTAL_XP" };

	clock_t start;
	double timeSeconds = 0;
	double timeMicroseconds = 0;
	double totalTime = 0;

	//randNums();
	readInput(skillData);

	if (strcmp(argv[1], "standard") == 0) {
		for (int i = 0; i < skills; i++) {
			printf("%s\n", skillType[i]);
			start = clock();

			qsort_r(skillData, playerSize, (skills * sizeof(int)), cmp, &i);

			timeSeconds = (double)(clock() - start) / CLOCKS_PER_SEC;
			timeMicroseconds = timeSeconds * 1000000.0;

			if (print == 1) {
				for (int j = 0; j < playerSize; j++) {
					printf("%d \n", skillData[j * skills + i]);
				}
			}
			printf("time taken: %f\n\n", timeMicroseconds);
			totalTime += timeMicroseconds;
		}

		printf("total time taken: %f\n", totalTime);
	}

	if (strcmp(argv[1], "custom") == 0) {
		int sdIndex;
		int* cArr = calloc(countingArraySize, sizeof(int));

		// using a max heap for large values
		int bMaxSize = (int)(playerSize*ratioOfLargeValues + 1);
		int* bArr = calloc(bMaxSize, sizeof(int));
		int arrSize = 0;

		for (int i = 0; i < skills; i++) {
			printf("%s\n", skillType[i]);
			start = clock();

			// sort stuff here
			for (int j = 0; j < playerSize; j++) {
				sdIndex = (j * skills) + i;
				if (skillData[sdIndex] >= countingArraySize) {
					
					bArr[arrSize] = skillData[sdIndex];
					heapUp(bArr, arrSize);
					arrSize++;
					
				} else {
					cArr[(skillData[sdIndex])]++;
				}
			}

			timeSeconds = (double)(clock() - start) / CLOCKS_PER_SEC;
			timeMicroseconds = timeSeconds * 1000000.0;

			// print stuff here
			if (print == 1) {
				for (int j = 0; j < arrSize; j++) {
					printf("%d\n", bArr[0]);
					bArr[0] = 0;
					heapDown(bArr, 0, arrSize);
				}

				for (int j = countingArraySize-1; j >= 0; j--) {
					while (cArr[j] > 0) {
						printf("%d\n", j);
						cArr[j]--;
					}
				}
			}

			printf("time taken: %f\n\n", timeMicroseconds);
			totalTime += timeMicroseconds;

			arrSize = 0;
			memset(bArr, 0, bMaxSize * sizeof(int));
		}

		printf("total time taken: %f\n", totalTime);
		free(cArr);
		free(bArr);
	}

	free(skillData);
	return 0;
} 