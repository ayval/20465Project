#include "datastructures.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*Adds a new label to the head of the linked list*/
int pushLabel(Label** head, char *newLabelName, int newLabelAddress)
{
	/*Allocate memory for the new label*/
	Label *newLabel = (Label *)malloc(sizeof(Label));
	
	/*Sanity check that malloc succeeded*/
	if (! newLabel) {
		return 0;
	}
	
	/*Insert the data*/
 	strcpy(newLabel->name,newLabelName);
	newLabel->address=newLabelAddress;
	/*Change next of new node as head */
	newLabel->next = (*head);
	/*Move the head to point to the new node */
	(*head) = newLabel;

	return 1;
}

/*search for the address of a specific label*/
int getAddressByName(Label **head, char *nameToSearch) {
	Label *temp;

	/*sanity check for empty list*/
	if (*head==NULL) {
		return FALSE;
	}
	temp = *head;
	
	/*Go over the list until the label is found*/
	while (temp) {
		if (! strcmp(temp->name,nameToSearch))
			return temp->address;
		temp = temp->next;
	}
	
	/*Nothing was found*/
	return FALSE;
}

/*Safe add of a label. Make sure there are no double references*/
int safePushLabel(Label **head, char *newLabelName, int newLabelAddress) {
	int result;
	if (! getAddressByName(head, newLabelName))
		result = pushLabel(head, newLabelName, newLabelAddress);
	return result;
}

/*not for product - debug only - print all labels*/
void printLabels(Label **head) {
	Label *temp;
	if (*head==NULL) {
		printf("Empty list\n");
		return;
	}
	
	temp = *head;
	while(temp) {
		printf("Label:%s Address:%d\n", temp->name, temp->address);
		temp=temp->next;
	}
}

/*
int main() {
	Label *head=NULL;
	int i = 0;
	safePushLabel(&head,"a",1);
	safePushLabel(&head,"b",2);
	safePushLabel(&head,"a",3);
	safePushLabel(&head,"d",4);
	i = getAddressByName(&head, "y");
	printLabels(&head);
}
*/
