void insere_ip (Indice *idx, void *chave)
{
	int filho_direito = -1;
	Chave_ip *chave_promovida;
	chave_promovida = NULL;

	if (idx-> raiz == NULL)
	{
		no_x = criar_no('p');
		no_x-> folha = 'F';
		no_x-> num_chaves = 1;
		strcpy (no_x-> chave[0].pk, chave_ip-> pk);
		no_x-> chave[0].rrn = chave_ip-> rrn;
		idx-> raiz = 0;
		write_btree (no_x, nregistrosip, 'p');
		libera_no (no_x, 'p');
		nregistrosip ++;
	}
	else
	{
		insere_aux_ip (no_x, chave, &filho_direito, chave_promovida);

		if (chave_promovida != NULL)
		{
			no_x = criar_no('p');
			no_x-> folha = 'F';
			no_x-> num_chaves = 1;
			strcpy (no_x-> chave[0].pk, chave_promovida-> pk);
			no_x-> chave[0].rrn = chave_promovida-> rrn;
			no_x-> desc[0] = idx-> raiz;
			no_x-> desc[0] = filho_direito;
			idx-> raiz = nregistrosip;
			write_btree (no_x, nregistrosip, 'p');
			libera_no (no_x, 'p');
			nregistrosip ++;
		}
	}
}

void insere_aux_ip (node_Btree_ip *no_x, Chave_ip *chave, int *filho_direito, Chave_ip *chave_promovida)
{
	int i;

	if (no_x-> folha == 'F')
	{
		if (no_x-> num_chaves < ordem_ip - 1)
		{
			for (i = no_x-> num_chaves - 1; i >= 0 && strcmp (chave-> pk, no_x-> chave[i].pk) < 0; i--)
			{
				no_x-> chave[i + 1].rrn = no_x-> chave[i].rrn;
				strcpy (no_x-> chave[i + 1].pk, no_x-> chave[i].pk);
			}
			no_x-> chave[i + 1].rrn = chave.rrn;	
			strcmp (no_x-> chave[i + 1].pk, chave.pk;);
			no_x-> num_chaves ++;
			*filho_direito = -1;
			chave_promovida = NULL;
			return;
		}
		else
		{
			divide_no_ip (no_x, chave, filho_direito, chave_promovida);
			return;
		}
	}
	else
	{
		for (i = no_x-> num_chaves - 1; i >= 0 && strcmp (chave-> pk, no_x-> chave[i].pk) < 0; i--){}
		i ++;
		insere_aux_ip (no_x-> desc[i], chave, &filho_direito, chave_promovida);

		if (chave_promovida != NULL)
		{
			chave-> rrn = chave_promovida-> rrn;
			strcpy (chave-> pk, chave_promovida-> pk);
			if (no_x-> num_chaves < ordem_ip - 1)
			{
				for (i = no_x-> num_chaves - 1; i >= 0 && strcmp (chave-> pk, no_x-> chave[i].pk) < 0; i--)
				{
					no_x-> chave[i + 1].rrn = no_x-> chave[i].rrn;
					strcpy (no_x-> chave[i + 1].pk, no_x-> chave[i].pk);
					no_x-> desc[i + 2] = no_x-> desc[i + 1];
				}
				no_x-> chave[i + 1].rrn = chave.rrn;
				strcpy (no_x-> chave[i + 1].pk, chave.pk);
				no_x-> desc[i + 2] = filho_direito;
				no_x-> num_chaves ++;
				write_btree (no_x, nregistrosip, 'p'); //??
				libera_no (no_x, 'p');
				*filho_direito = -1;
				chave_promovida = NULL;
			}
			else
			{
				divide_no_ip (node_Btree_ip *no_x, chave, filho_direito, chave_promovida);
				return;
			}
		}
		else
		{
			write_btree (no_x, nregistrosip, 'p'); //??
			libera_no (no_x, 'p');
			*filho_direito = -1;
			chave_promovida = NULL;
			return;
		}
	}
}

void divide_no_ip (no_x, Chave_ip *chave, int *filho_direito, Chave_ip *chave_promovida)
{
	int chave_alocada = 0;
	i = no_x-> num_chaves - 1;

	no_y = criar_no('p');
	no_y-> folha = no_x-> folha;
	no_y-> num_chaves = (ordem_ip - 1) / 2;

	for (int j = no_y-> num_chaves - 1; i >= 0; i--)
	{
		if (chave_alocada == 0 && strcmp (chave.pk, no_x-> chave[i].pk) > 0)
		{
			no_y-> chave[j].rrn = chave.rrn; 
			strcpy (no_y-> chave[j].pk, chave.pk);
			no_y-> desc[j + 1] = filho_direito;

		}
		else
		{
			no_y-> chave[j].rrn = no_x-> chave[i].rrn; 
			strcpy (no_y-> chave[j].pk, no_x-> chave[i].pk);
			no_y-> desc[j + 1] = no_x-> desc[i + 1];
			i --;
		}
	}
	if (!chave_alocada)
	{
		while (i >= 0 && strcmp(chave_ip-> pk, no_x-> chave[i].pk) < 0)
		{
			strcpy (no_x-> chave[i + 1].pk, no_x-> chave[i].pk);
			no_x-> desc[i + 2] = no_x-> desc[i + 1]; 
			i --;
		}
		strcpy (no_x-> chave[i + 1].pk, chave_ip-> pk);
		no_x-> chave[i + 1].rrn = chave_ip-> rrn;
		no_x-> desc[i + 2] = *filho_direito;
	}
	
	strcpy (chave_promovida_ip-> pk, no_x-> chave[ordem_ip / 2].pk);
	chave_promovida_ip-> rrn = no_x-> chave[(ordem_ip / 2) + 1].rrn;
	novo_no-> desc[0] = no_x-> desc[(int)(ordem_ip / 2) + 2];
	no_x-> num_chaves = (int)(ordem_ip / 2);
	write_btree (novo_no, nregistrosip, 'p');
	libera_no (novo_no, 'p');
	*filho_direito = nregistrosip;
	nregistrosip ++;
	write_btree (no_x, nregistrosip, 'p');
	return;
}