while(head->next->data != -1)
	{
		if(req_cycle == cycle)
		{
			empty_dram = true;
			if(prev_dram_ins!=-1)
			{free(prev_dram_ins,R_used,busy);prev_dram_ins = -1;}

			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				prev_dram_ins = ins;

				int add = temp->saved_address;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				req_cycle = cycle + col_delay;
				
				cout<<"line number "<<ins<<" : cycle "<<cycle<<" - "<<req_cycle-1;

				if((memory_instruction>>26 & ((1<<5)-1))== 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;
				
				cycle++;
				continue;
			}
			else
			{
				Node* temp = head -> next;
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
				
				cout<<"line number "<<ins<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"line number "<<ins<<" : cycle "<<cycle<<" - "<<req_cycle-1;
				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;

				num_sw = 0;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				cycle++;
				continue;
			}
		}
		
		cycle = req_cycle;
	}

	if(prev_dram_ins!=-1)
	{free(prev_dram_ins,R_used,busy);prev_dram_ins = -1;}

	if(num_sw!=0)
	{
		write_row(memory,buffer,buffer_row);
		req_cycle = cycle + row_delay+1;
		cout << "Final writeback to memory : cycle "<<cycle+1 <<" - " <<req_cycle-1<<endl;
		row_updates++;	
	}

	cout<<"\nTotal number of cycles : "<<max(cycle,req_cycle)-1<<endl<<"Total number of row buffer updates : "<<row_updates<<endl;
	return 0;