#include "datastructures.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*Adds a new label to the head of the linked list*/
int pushLabel(Label** head, char *newLabelName, int newLabelAddress, LabelType lType)
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
	newLabel->type=lType;
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
int safePushLabel(Label **head, char *newLabelName, int newLabelAddress, LabelType lType) {
	int result;
	if (! getAddressByName(head, newLabelName))
		result = pushLabel(head, newLabelName, newLabelAddress, lType);
	return result;
}


/*update all data labels with constant*/
int updateDataCounterLabels(Label **head, int IC) {
	Label *temp;
	if (*head==NULL) 
		return FALSE;
	temp = *head;
	while (temp) {
		if (temp->type==data || temp->type==string || temp->type==structlabel) {
			temp->address+=IC;
		}
		temp=temp->next;
	}
	return TRUE;
}


/*not for production - debug only - print all labels*/
void printLabels(Label **head) {
	Label *temp;
	int i;
	if (*head==NULL) {
		printf("Empty list\n");
		return;
	}
	
	temp = *head;
	while(temp) {
		printf("Label:%s Address:%d Type: %d", temp->name, temp->address, temp->type);
		if (temp->type==data) {
			printf(" data: ");
			for (i=0; i<temp->data.length; i++)
				printf("%d ",temp->data.data[i]);
			printf("\n");
		}
		if (temp->type==structlabel) {
			printf(" struct: number: %d string: %s\n", temp->aStruct.number, temp->aStruct.data);
		}
		if (temp->type==string) {
			printf(" string: %s\n", temp->aStruct.data);
		}
		temp=temp->next;
	}
}


/*add the ints to the data label struct*/
int updateLabelData(Label **head, AData *aData, char *labelName) {
	int i;
	Label *temp;	
	/*sanity check for empty list*/
	if (*head==NULL) {
		return FALSE;
	}
	temp = *head;

	while (temp) {
		if (! strcmp(temp->name,labelName)) {
			for (i=0; i<aData->length; i++)
				temp->data.data[i] = aData->data[i];
				temp->data.length = aData->length;
			return TRUE;
		}
		temp = temp->next;
	}
	return FALSE;
}


int updateLabelString(Label **head, char *stringValue, char *labelName) {
	Label *temp;	
	/*sanity check for empty list*/
	if (*head==NULL) {
		return FALSE;
	}
	temp = *head;

	while (temp) {
		if (! strcmp(temp->name,labelName)) {
			strcpy(temp->aStruct.data, stringValue);
			return TRUE;
		}
		temp=temp->next;
	}
	return FALSE;
}


int updateLabelStruct(Label **head, AStruct *analyzedStruct, char *labelName) {
	Label *temp;	
	/*sanity check for empty list*/
	printf("inside struct update\n");
	if (*head==NULL) {
		return FALSE;
	}
	printf("inside struct update after sanity\n");
	temp = *head;
	while (temp) {
		if (! strcmp(temp->name,labelName)) {
			printf("label: %s the analyzed struct data is: %d, %s\n", labelName, analyzedStruct->number, analyzedStruct->data);
			strcpy(temp->aStruct.data, analyzedStruct->data);
			temp->aStruct.number = analyzedStruct->number;
			return TRUE;
		}
		temp=temp->next;
	}
	return FALSE;
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
