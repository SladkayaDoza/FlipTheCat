#pragma once
#include "si_types.h"
#include "si_game_objects.h"
#include "si_data.h"
#include "SPIFFS.h"
#include "FS.h"

// ============ GRAPHICS ENGINE ============

static Uint8 si_pmSpace[804], si_pmIntro[531], si_pmImpact[912];
static Uint8 si_pmNum[10][15] = {{1,1,1,1,0,1,1,0,1,1,0,1,1,1,1},{0,0,1,0,0,1,0,0,1,0,0,1,0,0,1},{1,1,1,0,0,1,1,1,1,1,0,0,1,1,1},{1,1,1,0,0,1,1,1,1,0,0,1,1,1,1},{1,0,1,1,0,1,1,1,1,0,0,1,0,0,1},{1,1,1,1,0,0,1,1,1,0,0,1,1,1,1},{1,1,1,1,0,0,1,1,1,1,0,1,1,1,1},{1,1,1,0,0,1,0,0,1,0,0,1,0,0,1},{1,1,1,1,0,1,1,1,1,1,0,1,1,1,1},{1,1,1,1,0,1,1,1,1,0,0,1,1,1,1}};
static Uint8 si_cmSpace[101] = {15,255,63,248,127,131,252,127,227,255,199,255,159,249,255,143,252,120,0,224,231,15,60,3,192,30,0,56,28,225,207,0,120,3,192,7,3,28,57,224,15,0,255,240,255,231,255,60,1,255,143,255,31,248,255,231,0,63,224,3,231,192,28,121,224,15,128,0,124,248,7,143,60,1,240,0,15,31,0,241,231,128,62,1,255,227,224,28,56,255,231,255,127,240,248,7,143,7,249,255,12};
static Uint8 si_cmIntro[65] = {0,0,0,0,0,0,45,193,128,0,0,14,0,2,244,27,0,0,2,120,0,191,168,240,0,0,62,156,11,219,41,128,0,9,228,242,220,163,192,0,0,224,125,0,1,176,0,0,32,19,192,0,96,0,0,0,1,192,0,0,0,0,0,0,2};
static Uint8 si_cmImpact[114] = {31,31,135,207,252,63,193,254,127,241,225,252,252,255,231,254,127,231,255,30,31,255,204,30,225,231,128,7,131,225,255,249,193,206,28,240,0,240,62,63,255,156,24,225,207,0,15,3,195,206,241,255,159,252,240,0,240,60,60,207,31,241,255,206,0,15,3,195,192,227,192,28,121,224,1,224,60,120,14,60,3,199,158,0,30,7,135,129,227,192,60,121,224,1,224,120,120,30,60,3,199,31,252,30,15,143,1,231,128,120,240,127,131,192};
static Uint8 si_pmScrollMark[21] = {1,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,1};
static Uint8 si_pmDotEmpty[12] = {0,1,1,0,1,0,0,1,0,1,1};
static Uint8 si_pmDotFull[12] = {0,1,1,0,1,1,1,1,0,1,1};
static Uint8 si_pmLife[25] = {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0,0,1};
static Uint8 si_pmMissileIcon[25] = {0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1,0,1,1};
static Uint8 si_pmBeamIcon[25] = {0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1};
static Uint8 si_pmWallIcon[25] = {0,1,1,1,0,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,0,1,1,1};
static Uint8 si_pmShot[3] = {1,1,1};
static Uint8 si_pmExplosion[2][25] = {{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},{0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1}};

static SI_Object SI_StaticObject[] = {
    {{3,5},si_pmNum[0]},{{3,5},si_pmNum[1]},{{3,5},si_pmNum[2]},{{3,5},si_pmNum[3]},{{3,5},si_pmNum[4]},
    {{3,5},si_pmNum[5]},{{3,5},si_pmNum[6]},{{3,5},si_pmNum[7]},{{3,5},si_pmNum[8]},{{3,5},si_pmNum[9]},
    {{67,12},si_pmSpace},{{59,9},si_pmIntro},{{76,12},si_pmImpact},{{3,7},si_pmScrollMark},
    {{4,3},si_pmDotEmpty},{{4,3},si_pmDotFull},{{5,5},si_pmLife},{{5,5},si_pmMissileIcon},
    {{5,5},si_pmBeamIcon},{{5,5},si_pmWallIcon},{{3,1},si_pmShot},{{5,5},si_pmExplosion[0]},{{5,5},si_pmExplosion[1]},
};
static SI_Object* SI_DynamicObject[256] = {NULL};

static void SI_UncompressPixelMap(Uint8* PixelMap, Uint16 Pixels, Uint16 Bytes) {
    Uint8 Bits = Pixels % 8;
    if (Bits == 0) Bits = 8;
    while (Bytes--) {
        while (Bits--) {
            PixelMap[Bytes * 8 + Bits] = PixelMap[Bytes] % 2;
            if (Bytes != 0 || Bits != 0) PixelMap[Bytes] >>= 1;
        }
        Bits = 8;
    }
}
static void SI_UncompressObject(Uint8* Compressed, int CompressedSize, Uint8* Container, int ContainerSize) {
    memcpy(Container, Compressed, CompressedSize);
    SI_UncompressPixelMap(Container, ContainerSize, CompressedSize);
}
static void SI_UncompressObjects() {
    SI_UncompressObject(si_cmSpace, sizeof(si_cmSpace), si_pmSpace, sizeof(si_pmSpace));
    SI_UncompressObject(si_cmIntro, sizeof(si_cmIntro), si_pmIntro, sizeof(si_pmIntro));
    SI_UncompressObject(si_cmImpact, sizeof(si_cmImpact), si_pmImpact, sizeof(si_pmImpact));
}
static void SI_UncompressFont() {
    for (Uint8 i = 33; i < 127; ++i) {
        memcpy(SI_SpaceFont[i], SI_CompressedFont[i - 33], 5);
        SI_UncompressPixelMap(SI_SpaceFont[i], 40, 5);
    }
}

static SI_Object SI_GetObject(Uint16 ObjectID) {
    if (ObjectID < 256) return SI_StaticObject[ObjectID];
    else {
        ObjectID %= 256;
        if (!SI_DynamicObject[ObjectID]) {
            Uint8 Size[2];
            const Uint8 *ObjectData = si_getObjectData(ObjectID);
            if (!ObjectData) return SI_NewObject(SI_NewVec2(0, 0), NULL);
            memcpy(Size, ObjectData, 2);
            Uint16 Pixels = (Uint16)Size[0] * (Uint16)Size[1];
            Uint16 Bytes = Pixels / 8 + (Pixels % 8 != 0);
            Uint8* NewPixelMap = (Uint8*)malloc(Pixels);
            if(!NewPixelMap) return SI_NewObject(SI_NewVec2(0, 0), NULL);
            memcpy(NewPixelMap, &ObjectData[2], Bytes);
            SI_UncompressPixelMap(NewPixelMap, Pixels, Bytes);
            SI_DynamicObject[ObjectID] = (SI_Object*)malloc(sizeof(SI_Object));
            if(!SI_DynamicObject[ObjectID]) { free(NewPixelMap); return SI_NewObject(SI_NewVec2(0, 0), NULL); }
            SI_DynamicObject[ObjectID]->Size.x = Size[0];
            SI_DynamicObject[ObjectID]->Size.y = Size[1];
            SI_DynamicObject[ObjectID]->Samples = NewPixelMap;
        }
        return *SI_DynamicObject[ObjectID];
    }
    return SI_NewObject(SI_NewVec2(0, 0), NULL);
}
static void SI_FreeDynamicGraphics() {
    for (int i = 0; i < 256; ++i) {
        if (SI_DynamicObject[i]) { free(SI_DynamicObject[i]->Samples); free(SI_DynamicObject[i]); SI_DynamicObject[i] = NULL; }
    }
}

static void SI_DrawObject(Uint8* PixelMap, SI_Object obj, SI_Vec2 pos) {
    int x=pos.x, ex=pos.x+obj.Size.x, y=pos.y, ey=pos.y+obj.Size.y;
    for (int px=x; px<ex; px++)
        for (int py=y; py<ey; py++)
            if (obj.Samples && obj.Samples[(py-y)*obj.Size.x+px-x])
                if (px<84 && py<48 && px>=0 && py>=0) PixelMap[py*84+px]=1;
}
static void SI_DrawOutlinedObject(Uint8* PixelMap, SI_Object obj, SI_Vec2 pos) {
    int x=pos.x, ex=pos.x+obj.Size.x, y=pos.y, ey=pos.y+obj.Size.y;
    for (int px=x; px<ex; px++)
        for (int py=y; py<ey; py++)
            if (obj.Samples && obj.Samples[(py-y)*obj.Size.x+px-x])
                if (px<84 && py<48 && px>=0 && py>=0) {
                    Uint16 P=py*84+px;
                    if(py){if(px)PixelMap[P-85]=0;PixelMap[P-84]=0;if(px!=83)PixelMap[P-83]=0;}
                    if(px)PixelMap[P-1]=0;if(px!=83)PixelMap[P+1]=0;
                    if(py!=47){if(px)PixelMap[P+85]=0;PixelMap[P+84]=0;if(px!=83)PixelMap[P+83]=0;}
                }
    SI_DrawObject(PixelMap, obj, pos);
}
static void SI_DrawSmallNumber(Uint8* PixelMap, Uint16 Num, Uint8 Digits, SI_Vec2 LastDigit) {
    while (Digits--) {
        SI_DrawObject(PixelMap, SI_GetObject((SI_Graphics)(Num%10)), SI_NewVec2(LastDigit.x, LastDigit.y));
        Num/=10; LastDigit.x-=4;
    }
}
static void SI_DrawText(Uint8* PixelMap, const char* Text, SI_Vec2 Pos, int LineHeight) {
    Sint16 PosX=Pos.x;
    while (*Text) {
        if (*Text=='\n') { Pos.x=PosX; Pos.y+=LineHeight; }
        else { SI_DrawObject(PixelMap, SI_NewObject(SI_NewVec2(5,8), SI_SpaceFont[(unsigned char)*Text]), Pos); Pos.x+=6; }
        ++Text;
    }
}
static void SI_InvertScreen(Uint8* PixelMap) { for(int i=0;i<4032;++i)PixelMap[i]=1-PixelMap[i]; }
static void SI_InvertScreenPart(Uint8* PixelMap, SI_Vec2 From, SI_Vec2 To) {
    Sint8 yFrom=From.y;
    for(;From.x<=To.x;++From.x)for(From.y=yFrom;From.y<=To.y;From.y++){Uint8*P=PixelMap+From.y*84+From.x;*P=1-*P;}
}
static void SI_DrawScrollBar(Uint8* PixelMap, Uint8 Percent) {
    for (Uint16 i=6*84+81; i<39*84+81; i+=84) PixelMap[i]=1;
    SI_InvertScreenPart(PixelMap, SI_NewVec2(81,Percent/4+7), SI_NewVec2(81,Percent/4+11));
    SI_DrawObject(PixelMap, SI_GetObject(SI_gScrollMark), SI_NewVec2(81,Percent/4+6));
}

// ============ SHOT LIST ============
static Uint8 SI_ShotDamages[4] = {1,3,10,25};
static SI_Vec2 SI_ShotSizes[4] = {{3,1},{5,3},{84,3},{1,43}};

static void SI_AddShot(SI_ShotList *Shots, SI_Vec2 Pos, Sint8 v, Uint8 FromPlayer, SI_WeaponKind Kind) {
    SI_Shot *NewShot=(SI_Shot*)malloc(sizeof(SI_Shot));
    if(!NewShot)return;  // куча исчерпана — просто не создаём выстрел (без разыменования NULL)
    NewShot->Pos=Pos;NewShot->v=v;NewShot->FromPlayer=FromPlayer;NewShot->Kind=Kind;
    NewShot->Damage=SI_ShotDamages[Kind];NewShot->Size=SI_ShotSizes[Kind];NewShot->Next=NULL;
    if(*Shots){SI_Shot *CreateAt=*Shots;while(CreateAt->Next)CreateAt=CreateAt->Next;CreateAt->Next=NewShot;}
    else *Shots=NewShot;
}
static void SI_EmptyShotList(SI_ShotList *Shots) {
    SI_Shot *Last=*Shots;while(*Shots){*Shots=(*Shots)->Next;free(Last);Last=*Shots;}*Shots=NULL;
}
static void SI_RemoveShot(SI_ShotList *Shots, SI_Shot *Address) {
    SI_Shot*Checked=*Shots,*Last=*Shots;
    while(Checked){if(Checked==Address){SI_Shot*Current=Checked;if(Checked==*Shots)*Shots=(*Shots)->Next;Last->Next=Current->Next;free(Current);return;}Last=Checked;Checked=Checked->Next;}
}
static Uint8 SI_Intersect(SI_Vec2 S1, SI_Vec2 Sz1, SI_Vec2 S2, SI_Vec2 Sz2) {
    return !(S1.x>S2.x+Sz2.x-1||S1.y>S2.y+Sz2.y-1||S1.x+Sz1.x-1<S2.x||S1.y+Sz1.y-1<S2.y);
}
static void SI_ShotListTick(SI_ShotList *Shots, Uint8 *PixelMap, SI_PlayerObject *Player) {
    SI_Shot*Current=*Shots;
    while(Current){
        Uint8 HitPlayer=!Current->FromPlayer&&SI_Intersect(Current->Pos,Current->Size,Player->Pos,SI_NewVec2(10,7));
        Uint8 HitOther=0; SI_Shot*CheckForHit=*Shots;
        Current->Pos.x+=Current->v;
        if(HitPlayer&&!Player->Protection)Player->Lives--;
        while(CheckForHit&&!HitOther){
            SI_Shot*NextCheck=CheckForHit->Next;  // запоминаем до возможного free (иначе чтение освобождённой памяти)
            HitOther=CheckForHit!=Current&&!(CheckForHit->FromPlayer&&Current->FromPlayer)&&SI_Intersect(Current->Pos,Current->Size,CheckForHit->Pos,CheckForHit->Size);
            if(HitOther){Current->Damage--;CheckForHit->Damage--;if(CheckForHit->Damage==0)SI_RemoveShot(Shots,CheckForHit);}
            CheckForHit=NextCheck;
        }
        if(Current->Pos.x<-2||Current->Pos.x>83||HitPlayer||Current->Damage==0){
            SI_Shot*Remove=Current;Current=Current->Next;SI_RemoveShot(Shots,Remove);
        } else {
            SI_DrawObject(PixelMap,SI_GetObject(Current->Kind==SI_Standard?SI_gShot:SI_G_MISSILE+Current->Kind-SI_Missile),Current->Pos);
            Current=Current->Next;
        }
    }
}

// ============ ENEMIES ============
static SI_Enemy* SI_EnemiesDB[256] = {NULL};

static SI_Enemy SI_GetEnemy(Uint8 ID) {
    if(!SI_EnemiesDB[ID]){
        // Сначала проверяем данные, только потом выделяем память и кэшируем.
        // Раньше malloc шёл ДО проверки: при неизвестном ID в кэше оставался
        // неинициализированный мусор, который возвращался при следующем вызове.
        const Uint8*ed=si_getEnemyData(ID);
        if(!ed){SI_Enemy E;memset(&E,0,sizeof(E));return E;}
        SI_Enemy*E=(SI_Enemy*)malloc(sizeof(SI_Enemy));
        if(!E){SI_Enemy Z;memset(&Z,0,sizeof(Z));return Z;}
        Uint8 ModelID=ed[0], MovesBetween[2];
        E->Model=ModelID+256;E->Size.x=E->Size.y=0;
        E->AnimCount=ed[1];E->Lives=ed[2];E->Floats=ed[3];
        E->ShotTime=ed[4];E->MoveUp=ed[5];E->MoveDown=ed[6];E->MoveAnyway=ed[7];
        memcpy(MovesBetween,&ed[8],2);
        E->MovesBetween.x=MovesBetween[0];E->MovesBetween.y=MovesBetween[1];
        for(Uint8 i=ModelID;i<ModelID+E->AnimCount;++i){
            SI_Object AP=SI_GetObject((Uint16)(i+256));  // размер по КАЖДОМУ кадру i (а не всегда по кадру 0)
            if(E->Size.x<AP.Size.x)E->Size.x=AP.Size.x;
            if(E->Size.y<AP.Size.y)E->Size.y=AP.Size.y;
        }
        SI_EnemiesDB[ID]=E;
    }
    return *SI_EnemiesDB[ID];
}
static void SI_FreeDynamicEnemies() { for(int i=0;i<256;i++)if(SI_EnemiesDB[i]){free(SI_EnemiesDB[i]);SI_EnemiesDB[i]=NULL;} }

static void SI_AddEnemy(SI_EnemyListStart *Enemies, SI_Vec2 Pos, Uint8 EnemyID, Sint8 MoveDir) {
    SI_EnemyList*NewEnemy=(SI_EnemyList*)malloc(sizeof(SI_EnemyList));
    if(!NewEnemy)return;  // куча исчерпана — не спавним врага (без разыменования NULL)
    NewEnemy->Pos=Pos;NewEnemy->Type=SI_GetEnemy(EnemyID);NewEnemy->AnimState=0;
    NewEnemy->Lives=NewEnemy->Type.Lives;NewEnemy->MoveDir=MoveDir;NewEnemy->Cooldown=NewEnemy->Type.ShotTime;NewEnemy->Next=NULL;
    if(*Enemies){SI_EnemyList*CreateAt=*Enemies;while(CreateAt->Next)CreateAt=CreateAt->Next;CreateAt->Next=NewEnemy;}
    else *Enemies=NewEnemy;
}
static void SI_RemoveEnemy(SI_EnemyListStart *List, SI_EnemyList *Address) {
    SI_EnemyList*Checked=*List,*Last=*List;
    while(Checked){if(Checked==Address){SI_EnemyList*Current=Checked;if(Checked==*List)*List=(*List)->Next;Last->Next=Current->Next;free(Current);return;}Last=Checked;Checked=Checked->Next;}
}
static void SI_EmptyEnemyList(SI_EnemyListStart *Enemies) {
    SI_EnemyList*Last=*Enemies;while(*Enemies){*Enemies=(*Enemies)->Next;free(Last);Last=*Enemies;}*Enemies=NULL;
}
static void SI_EnemyListTick(SI_EnemyListStart *Enemies, SI_PlayerObject *Player, Uint8 *PixelMap, SI_ShotList *Shots, Uint8 AnimPulse, Uint8 *MoveScene) {
    SI_Shot*CheckedShot;SI_EnemyList*Current=*Enemies;
    while(Current){
        Uint8 Alive=Current->Lives>0;Uint8 InScreen=Current->Pos.x<=60;
        if(!Current->Next&&InScreen)*MoveScene=0;
        if((Current->Type.MoveUp||Current->Type.MoveDown)&&(InScreen||Current->Type.MoveAnyway)){
            if(Current->MoveDir==1&&Current->Pos.y==Current->Type.MovesBetween.y)Current->MoveDir=-Current->Type.MoveUp;
            else if(Current->MoveDir==-1&&Current->Pos.y==Current->Type.MovesBetween.x)Current->MoveDir=Current->Type.MoveDown;
            Current->Pos.y+=Current->MoveDir;
        }
        if(!Current->Type.Floats||!InScreen)Current->Pos.x--;
        if(AnimPulse)Current->AnimState=(Current->AnimState+1)%Current->Type.AnimCount;
        if(Alive){Uint8 GotHit=0;CheckedShot=*Shots;
            while(CheckedShot&&!GotHit){if(CheckedShot->FromPlayer){
                if(SI_Intersect(CheckedShot->Pos,CheckedShot->Size,Current->Pos,Current->Type.Size)){GotHit=1;
                    #ifdef BONUS_COLLIDER
                    if(Current->Type.Lives==127){Player->Score+=CheckedShot->Damage*5;SI_RemoveShot(Shots,CheckedShot);}else{
                    #else
                    if(Current->Type.Lives!=127){
                    #endif
                        Current->Lives-=CheckedShot->Damage;Player->Score+=5;
                        if(Current->Lives<0){CheckedShot->Damage=-Current->Lives;Current->Lives=0;}else SI_RemoveShot(Shots,CheckedShot);
                        if(Current->Lives==0){Player->Score+=5;Alive=0;Current->Type.MoveUp=Current->Type.MoveDown=0;}
                    }
                }}if(!GotHit)CheckedShot=CheckedShot->Next;
            }
        }
        if(Alive&&SI_Intersect(Player->Pos,SI_NewVec2(10,7),Current->Pos,Current->Type.Size)){
            Alive=0;
            if(Current->Type.Lives==127){
                SI_WeaponKind NewKind=(SI_WeaponKind)(rand()%3+1);
                if(NewKind!=Player->Weapon){Player->Bonus=0;Player->Weapon=NewKind;}
                Player->Bonus+=4-(int)NewKind;Current->Lives=-2;
            }else if(Player->Lives&&!Player->Protection)Player->Lives--;
        }
        if(Current->Pos.x<-Current->Type.Size.x||!Alive){
            Current->Lives--;
            if(Current->Lives==-3){SI_EnemyList*Remove=Current;Current=Current->Next;SI_RemoveEnemy(Enemies,Remove);continue;}
            else{SI_Vec2 AC=SI_NewVec2(Current->Pos.x+Current->Type.Size.x/2,Current->Pos.y+Current->Type.Size.y/2);
                SI_DrawObject(PixelMap,SI_GetObject(SI_gExplosionA1-Current->Lives-1),SI_NewVec2(AC.x-3-Current->Lives,AC.y-2));}
        }else{if(Current->Pos.x<84){SI_DrawObject(PixelMap,SI_GetObject(Current->Type.Model+Current->AnimState),Current->Pos);
            if(Current->Type.ShotTime){Current->Cooldown--;if(Current->Cooldown==0){SI_AddShot(Shots,SI_NewVec2(Current->Pos.x-1,Current->Pos.y+(Current->Type.Size.y/2)),-2,0,SI_Standard);Current->Cooldown=Current->Type.ShotTime;}}}}
        Current=Current->Next;
    }
    CheckedShot=*Shots;
    while(CheckedShot){SI_Shot*LastShot=CheckedShot;
        if(CheckedShot->Kind==SI_Missile){
            Uint8 TargetHeight=CheckedShot->Pos.y;SI_EnemyList*Cur=*Enemies;
            while(Cur){if(Cur->Pos.x>84)Cur=NULL;else if(Cur->Pos.x>CheckedShot->Pos.x){TargetHeight=Cur->Pos.y;Cur=NULL;}else Cur=Cur->Next;}
            if(CheckedShot->Pos.y!=TargetHeight){if(CheckedShot->Pos.y<TargetHeight)CheckedShot->Pos.y++;else CheckedShot->Pos.y--;}
        }
        CheckedShot=CheckedShot->Next;if(LastShot->Kind==SI_Beam)SI_RemoveShot(Shots,LastShot);
    }
}
static void SI_LevelSpawner(SI_EnemyListStart *Enemies, Uint8 Level) {
    const Uint8*LD=si_getLevelData(Level);if(!LD)return;
    SI_EmptyEnemyList(Enemies);Uint8 EC=LD[0];const Uint8*LDE=&LD[1];
    while(EC--){Uint8 ED[5];memcpy(ED,LDE,5);SI_AddEnemy(Enemies,SI_NewVec2(ED[0]*256+ED[1],ED[2]),ED[3],(Sint8)ED[4]-1);LDE+=5;}
}

// ============ SCENERY ============
static SI_SceneryData SI_ScData[6] = {{0,0,0},{256+0,2,0},{256+2,6,0},{256+8,6,0},{256+14,4,1},{256+14,4,1}};

static void SI_EmptyScenery(SI_SceneryList *List) {
    SI_Scenery*Last=*List;while(*List){*List=(*List)->Next;free(Last);Last=*List;}*List=NULL;
}
static void SI_HandleScenery(SI_SceneryList *List, Uint8 *PixelMap, Uint8 Move, SI_PlayerObject *Player, Sint8 Level) {
    SI_Scenery*First=*List;Sint16 LastX=0;SI_Object Model;
    while(*List){
        if(Move)(*List)->Pos.x--;
        Model=SI_GetObject((*List)->Model);
        if(Level!=1&&SI_Intersect((*List)->Pos,Model.Size,Player->Pos,SI_NewVec2(10,7)))Player->Lives--;
        if((*List)->Pos.x<-Model.Size.x){*List=(*List)->Next;free(First);First=*List;}
        else{LastX=(*List)->Pos.x+Model.Size.x;SI_DrawObject(PixelMap,Model,(*List)->Pos);*List=(*List)->Next;}
    }
    if(Level!=0){
        while(LastX<84){
            SI_Scenery*NS=(SI_Scenery*)malloc(sizeof(SI_Scenery));
            if(!NS)break;  // куча исчерпана — прекращаем достраивать пейзаж
            NS->Model=SI_ScData[Level].FirstObject+random(SI_ScData[Level].Objects);
            Model=SI_GetObject(NS->Model);
            NS->Pos=SI_NewVec2(LastX,SI_ScData[Level].Upper?0:48-Model.Size.y);NS->Next=NULL;
            LastX=NS->Pos.x+Model.Size.x;
            if(First==NULL)First=NS;else{for(*List=First;(*List)->Next;*List=(*List)->Next);(*List)->Next=NS;}
        }
    }
    *List=First;
}

// ============ SAVES (SPIFFS) ============
static void SI_ReadSavedLevel(Uint8 *Level) {
    File f=SPIFFS.open("/si_level.sav","r");if(f){f.read(Level,1);f.close();}
}
static void SI_ReadTopScore(unsigned int *Arr) {
    File f=SPIFFS.open("/si_score.sav","r");if(f){f.read((uint8_t*)Arr,SI_SCORE_COUNT*sizeof(unsigned int));f.close();}
}
static void SI_SaveLevel(Uint8 Level) {
    File f=SPIFFS.open("/si_level.sav","w");if(f){f.write(&Level,1);f.close();}
}
static void SI_PlaceTopScore(unsigned int *Arr, Uint16 Entry) {
    unsigned int*Start=Arr,*End=Arr+SI_SCORE_COUNT;
    // Сдвигаем только элементы НИЖЕ позиции вставки (j > insertPos, а не >=),
    // иначе при вставке нового рекорда на #1 читался Start[-1] (выход за границу).
    while(Arr!=End){if(*Arr<Entry){for(int j=SI_SCORE_COUNT-1;j>(int)(Arr-Start);j--)Start[j]=Start[j-1];*Arr=Entry;Arr=End;}else++Arr;}
    File f=SPIFFS.open("/si_score.sav","w");if(f){f.write((uint8_t*)Start,SI_SCORE_COUNT*sizeof(unsigned int));f.close();}
}
