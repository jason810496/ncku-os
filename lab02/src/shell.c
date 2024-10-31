#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../include/command.h"
#include "../include/builtin.h"

// ======================= requirement 2.3 =======================
/**
 * @brief 
 * Redirect command's stdin and stdout to the specified file descriptor
 * If you want to implement ( < , > ), use "in_file" and "out_file" included the cmd_node structure
 * If you want to implement ( | ), use "in" and "out" included the cmd_node structure.
 *
 * @param p cmd_node structure
 * 
 */
void redirection(struct cmd_node *p){
	if (p->in_file) {
		int in_fd = open(p->in_file, O_RDONLY);
		if (in_fd == -1) {
			perror("open in_file");
			exit(EXIT_FAILURE);
		}
		if (dup2(in_fd, STDIN_FILENO) == -1) {
			perror("dup2 in_file");
			exit(EXIT_FAILURE);
		}
		close(in_fd);
	}

	if (p->out_file) {
		int out_fd = open(p->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1) {
			perror("open out_file");
			exit(EXIT_FAILURE);
		}
		if (dup2(out_fd, STDOUT_FILENO) == -1) {
			perror("dup2 out_file");
			exit(EXIT_FAILURE);
		}
		close(out_fd);
	}

	if (p->in != STDIN_FILENO) {
		if (dup2(p->in, STDIN_FILENO) == -1) {
			perror("dup2 in");
			exit(EXIT_FAILURE);
		}
		close(p->in);
	}

	if (p->out != STDOUT_FILENO) {
		if (dup2(p->out, STDOUT_FILENO) == -1) {
			perror("dup2 out");
			exit(EXIT_FAILURE);
		}
		close(p->out);
	}
}
// ===============================================================

// ======================= requirement 2.2 =======================
/**
 * @brief 
 * Execute external command
 * The external command is mainly divided into the following two steps:
 * 1. Call "fork()" to create child process
 * 2. Call "execvp()" to execute the corresponding executable file
 * @param p cmd_node structure
 * @return int 
 * Return execution status
 */
int spawn_proc(struct cmd_node *p)
{
	// fork 
	pid_t pid = fork();
	if (pid < 0){
		perror("fork");
	}
	else if (pid == 0){
		// child process
		redirection(p);
		if (execvp(p->args[0], p->args) == -1){
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	else{
		// parent process
		int status;
		waitpid(pid, &status, 0);
	}
  	return 1;
}
// ===============================================================


// ======================= requirement 2.4 =======================
/**
 * @brief 
 * Use "pipe()" to create a communication bridge between processes
 * Call "spawn_proc()" in order according to the number of cmd_node
 * @param cmd Command structure  
 * @return int
 * Return execution status 
 */
int fork_cmd_node(struct cmd *cmd)
{
	int pipe_fd[2];
	int in = STDIN_FILENO;
	int out = STDOUT_FILENO;
	struct cmd_node *temp = cmd->head;
	while(temp != NULL){
		if (pipe(pipe_fd) == -1){
			perror("pipe");
			exit(EXIT_FAILURE);
		}
		temp->out = pipe_fd[1];
		if(temp->next != NULL){
			temp->next->in = pipe_fd[0];
		}
		else{
			temp->out = STDOUT_FILENO;
		}
		spawn_proc(temp);
		close(pipe_fd[1]);
		temp = temp->next;
	}
	return 1;
}
// ===============================================================


void shell()
{
	while (1) {
		printf(">>> $ ");
		char *buffer = read_line();
		if (buffer == NULL)
			continue;

		struct cmd *cmd = split_line(buffer);
		
		int status = -1;
		// only a single command
		struct cmd_node *temp = cmd->head;
		
		if(temp->next == NULL){
			status = searchBuiltInCommand(temp);
			if (status != -1){
				int in = dup(STDIN_FILENO), out = dup(STDOUT_FILENO);
				if( in == -1 | out == -1)
					perror("dup");
				redirection(temp);
				status = execBuiltInCommand(status,temp);

				// recover shell stdin and stdout
				if (temp->in_file)  dup2(in, 0);
				if (temp->out_file){
					dup2(out, 1);
				}
				close(in);
				close(out);
			}
			else{
				//external command
				status = spawn_proc(cmd->head);
			}
		}
		// There are multiple commands ( | )
		else{
			
			status = fork_cmd_node(cmd);
		}
		// free space
		while (cmd->head) {
			
			struct cmd_node *temp = cmd->head;
      		cmd->head = cmd->head->next;
			free(temp->args);
   	    	free(temp);
   		}
		free(cmd);
		free(buffer);
		
		if (status == 0)
			break;
	}
}
