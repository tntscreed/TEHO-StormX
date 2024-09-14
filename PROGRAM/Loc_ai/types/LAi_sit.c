/*
Тип: стоячий, всегда стоит, отвечает на диалоги, никогда не боится

	Используемые шаблоны:
		stay
		dialog
*/



#define LAI_TYPE_SIT		"sit"


//Инициализация
void LAi_type_sit_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = LAI_TYPE_SIT;
	chr.chr_ai.type.wait = 1.0;
	LAi_tmpl_stay_InitTemplate(chr);
	//дадим предметы для использования в таверне
	chr.TiedItems.itm1.model = "HandsItems\meet";
	chr.TiedItems.itm1.locator = "Saber_hand";	
	chr.TiedItems.itm2.model = "HandsItems\cup";
	chr.TiedItems.itm2.locator = "Saber_hand";
	//Установим анимацию персонажу
	LAi_SetDefaultSitAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_sit_CharacterUpdate(aref chr, float dltTime)
{
	float time = stf(chr.chr_ai.type.wait) - dltTime; 
	if (time < 0.0)
	{		
		ref chrTarget;
		int iTemp, i;
		float fAng;
		aref tmpl;
		float xAng = 0;
		float zAng = 0;
		float   locx, locy, locz;
		if(chr.chr_ai.tmpl != LAI_TMPL_DIALOG && !CheckAttribute(chr, "nonTable"))
		{
			time = 20;
			int num = FindNearCharacters(chr, 2.3, -1.0, 0.0, 0.0, false, true);
			for(i = 0; i < num; i++)
			{
				iTemp = chrFindNearCharacters[i].index;
				chrTarget = &characters[iTemp];
				if(chrTarget.chr_ai.type == LAI_TYPE_SIT && chrFindNearCharacters[i].index != nMainCharacterIndex)
				{
					GetCharacterAy(chr, &fAng);
					//xAng = stf(chrFindNearCharacters[i].dx) * cos(fAng) - stf(chrFindNearCharacters[i].dz) * sin(fAng);
					zAng = stf(chrFindNearCharacters[i].dz) * cos(fAng) + stf(chrFindNearCharacters[i].dx) * sin(fAng);				
					if (zAng > -0.9 && zAng < 0.9)
					{
						if(LAi_Character_CanDialog(chr, chrTarget) && rand(1))
						{				
							if (!LAi_tmpl_SetDialog(chr, chrTarget, 50.0)) return;
							if (!LAi_tmpl_SetDialog(chrTarget, chr, 50.0)) return;
							time = 60.0;
							break;
						}
						else time = 30.0;
					}
				}
			}
			if (CheckAttribute(chr, "standUp") && !LAi_IsCapturedLocation) //не при захвате колонии
			{
				num = FindNearCharacters(chr, 15.0, -1.0, -1.0, 0.01, false, false);
				int iNumEnemy = LAi_type_sit_FindEnemy(chr, num);
				if (iNumEnemy > 0) //есть враги
				{
					//Нападаем на новую цель
					GetCharacterPos(chr, &locx, &locy, &locz);	
					LAi_SetWarriorTypeNoGroup(chr); //ребрендинг и вперед на врага
					ChangeCharacterAddressGroup(chr, chr.location, "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
					LAi_group_Attack(chr, pchar);
				}
			}
			//слежение залезания ГГ в боксы
			if (CheckAttribute(chr, "watchBoxes") && chr.chr_ai.tmpl != LAI_TMPL_DIALOG) 
			{
				num = FindNearCharacters(chr, 10.0, -1.0, 180.0, 0.01, true, true);
				for(i = 0; i < num; i++)
				{
					if(nMainCharacterIndex == sti(chrFindNearCharacters[i].index))
					{					
						//нашли ГГ, проверяем, не в сундуке ли.						
						if (bMainCharacterInBox)
						{
							//Нападаем на новую цель
							GetCharacterPos(chr, &locx, &locy, &locz);	
							LAi_SetWarriorTypeNoGroup(chr); //ребрендинг и вперед на врага
							ChangeCharacterAddressGroup(chr, chr.location, "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
							LAi_group_Attack(chr, pchar);
						}
					}
				}
			}
		}
	}
	chr.chr_ai.type.wait = time;
}

//Загрузка персонажа в локацию
bool LAi_type_sit_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_sit_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_sit_TemplateComplite(aref chr, string tmpl)
{
	LAi_tmpl_player_InitTemplate(chr);
}

//Сообщить о желании завести диалог
void LAi_type_sit_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_sit_CanDialog(aref chr, aref by)
{
	if(sti(by.index) == nMainCharacterIndex && chr.chr_ai.tmpl == LAI_TMPL_DIALOG)
	{
		if(LAi_tmpl_dialog_StopNPC(chr)) return true;
	}	
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	//Согласимся на диалог
	return true;
}

//Начать диалог
void LAi_type_sit_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_tmpl_stay_InitTemplate(chr);
	LAi_tmpl_SetActivatedDialog(chr, by);
	//LAi_tmpl_dialog_NoAni(chr);
}

//Закончить диалог
void LAi_type_sit_EndDialog(aref chr, aref by)
{
	LAi_tmpl_stay_InitTemplate(chr);
	chr.chr_ai.type.wait = 15;
}


//Персонаж выстрелил
void LAi_type_sit_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{
}


//Персонаж атакован
void LAi_type_sit_Attacked(aref chr, aref by)
{
	
}

int LAi_type_sit_FindEnemy(aref chr, int num)
{
	if(num <= 0) return -1;
	int i, idx;
	if(LAi_grp_playeralarm > 0)
	{
		for(i = 0; i < num; i++)
		{
			idx = sti(chrFindNearCharacters[i].index);
			if(LAi_group_IsEnemy(chr, &Characters[idx])) return idx;
		}
	}
	else
	{		
		for(i = 0; i < num; i++)
		{
			idx = sti(chrFindNearCharacters[i].index);
			if(LAi_CheckFightMode(&Characters[idx])) return idx;
		}
	}
	return -1;
}
