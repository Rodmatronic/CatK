#pragma once

int v_createfile(char* filepath, char* content);
int v_readfile(char* filepath);
int v_rmfile(char* filepath);
int v_createdir(char* filepath);
int v_sync();
void vfs_init();
int v_listdir(char* filepath);