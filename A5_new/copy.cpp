while(cycle<=M)
	{
		if(req_cycle == cycle && buffer_row!=-1)
		{
			if(prev_dram_ins!=-1)		//there is actually a dram instuction just finished.
			{free(prev_dram_ins,R_used,busy[(core_of_ins[prev_dram_ins])]);prev_dram_ins = -1;}

			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				prev_dram_ins = ins;			//instruction in progress.

				int add = temp->saved_address;		//address with which it was saved.

				temp->prev->next = temp->next;		//delete node from common queue in O(1)
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;		//if sw instruction.

				req_cycle = cycle + col_delay;		
				
				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle<<" - "<<req_cycle-1;	//for printing.
				
				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					if(empty_dram) cycle++;
					empty_dram = false;
					break;
				}

				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[core_of_ins[ins]][(R_used[ins])]<<endl;

				empty_dram = false;					//dram is busy.
				continue;
			}
			else if(head->next->data != -1)		//common queue is not empty and buffer_row has no ins.
			{
				Node* temp = head -> next;		//selection for next row instruction.
				
				int ins = temp-> data;
				prev_dram_ins = ins;
				int add = temp->saved_address;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				int memory_instruction = memory[ins/256][ins%256];
								
				if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle = cycle + 2*row_delay+ col_delay+1;
					row_updates++;
				}
				else req_cycle = cycle + row_delay+ col_delay+1;

				buffer_row = add/1024;
				same_row[buffer_row].pop();
				copy_row(memory,buffer,buffer_row);

				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					if(empty_dram) cycle++;
					empty_dram = false;
					break;
				}

				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle+1<<" - "<<req_cycle-1;

				if((memory_instruction>>26 & ((1<<5)-1))== 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[core_of_ins[ins]][(R_used[ins])]<<endl;

				num_sw = 0;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				empty_dram = false;
				continue;
			}
			else empty_dram = true;
		}
		cycle = max(req_cycle,cycle+1);
	}
	cout<<endl;
	if(pending_instruction!=-1)
	{cout<<"Instruction : "<<pending_instruction<<" of core : "<<core_of_ins[pending_instruction]<<" has not completed in execution and will ";
	cout<<"finish at cycle : "<<pending_finish<<endl<<endl;}

	cout<<"The instructions remaining in dram which have not been executed are \n";
	int first = 0;
	while(head->next->data != -1)
	{	
		first = 1;
		Node* temp = head -> next;		//selection for next row instruction.
				
		int ins = temp-> data;
		
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<endl;
	}
	if(first==0){cout<<"NONE\n\n";}
	else cout<<endl;
	//cerr << memory[4][982];
	for(int i=0;i<N;i++)
	{
		if(core_remaining.find(i)!=core_remaining.end())
		{cout<<"Remaining number of instructions in core :"<<i<<" were "<<exit_instruction[i]-cur_instruction[i]<<endl;}
		else
		{cout<<"Core : "<<i<<" completed successfully"<<endl;}
	}

	cout<<"\nTotal number of cycles : "<<min(max(cycle,req_cycle)-1,M)<<endl<<"Total number of row buffer updates : "<<row_updates<<endl;