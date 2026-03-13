//> Chunks of Bytecode chunk-c
#include <stdlib.h>

#include "chunk.h"
//> chunk-c-include-memory
#include "memory.h"
//< chunk-c-include-memory
//> Garbage Collection chunk-include-vm
#include "vm.h"
//< Garbage Collection chunk-include-vm



void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
//> chunk-null-lines
  chunk->lines = NULL;
//< chunk-null-lines
//> chunk-init-constant-array
  initValueArray(&chunk->constants);
//< chunk-init-constant-array
}
//> free-chunk
void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
//> chunk-free-lines
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
//< chunk-free-lines
//> chunk-free-constants
  freeValueArray(&chunk->constants);
//< chunk-free-constants
  initChunk(chunk);
}
//< free-chunk
/* Chunks of Bytecode write-chunk < Chunks of Bytecode write-chunk-with-line
void writeChunk(Chunk* chunk, uint8_t byte) {
*/
//> write-chunk
//> write-chunk-with-line
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
//< write-chunk-with-line
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code,
        oldCapacity, chunk->capacity);
//> write-chunk-line
    chunk->lines = GROW_ARRAY(int, chunk->lines,
        oldCapacity, chunk->capacity);
//< write-chunk-line
  }

  chunk->code[chunk->count] = byte;
//> chunk-write-line
//If this instruction is on the same line as the previous run, just increment the count
  if (chunk->lineCount > 0 &&
      chunk->lines[chunk->lineCount - 1].line == line) {
    
        chunk->lines[chunk->lineCount - 1].count++;
        return;
  }
  //Otherwise, start a new run for this line
  else {
    //grow capacity if needed
    if (chunk->lineCapacity < chunk->lineCount) {
      int oldLineCapacity = chunk->lineCapacity;
      chunk->lineCapacity = GROW_CAPACITY(oldLineCapacity);
      chunk->lines = GROW_ARRAY(int, chunk->lines, oldLineCapacity, chunk->lineCapacity);
    }

    chunk->lines[chunk->lineCount].line = line;
    chunk->lines[chunk->lineCount].count = 1;
    
    chunk->lineCount++;
  }
//< chunk-write-line
  chunk->count++;
}
//< write-chunk

void getLine(Chunk* chunk, int instruction) {
  int line = 0;
  int count = 0;
  for (int i = 0; i < chunk->lineCount; i++) {
    count += chunk->lines[i].count;
    if (instruction < count) {
      line = chunk->lines[i].line;
      break;
    }
  }
  return line;

}

//> add-constant
int addConstant(Chunk* chunk, Value value) {
//> Garbage Collection add-constant-push
  push(value);
//< Garbage Collection add-constant-push
  writeValueArray(&chunk->constants, value);
//> Garbage Collection add-constant-pop
  pop();
//< Garbage Collection add-constant-pop
  return chunk->constants.count - 1;
}
//< add-constant
