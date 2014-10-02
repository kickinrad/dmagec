#dmagec
A C++ console application for managing a tabletop role-playing game.

###database tables

    CHARACTERS(ID,NAME,REALNAME,CURHP,TOTALHP,XP)
    SCENES(ID,NAME)
    CHARACTERSINSCENE(KEY,SCENE_ID,CHAR_ID)
    DAMAGEINSCENE(KEY,SCENE_ID,SRC_ID,TAR_ID,VAL)
    CHARACTERINVENTORY(KEY,CHAR_ID,ITEM_ID)
    ITEMS(ID,NAME,DESC)

###stuff to do
1. ~~implement the rest of console functions not reliant on more database tables~~
    * ~~easy~~
        * `sceneinfo`
    * ~~a little harder~~
        * `editscene`
        * `editchar`

* ~~implement more database tables~~
    * `CHARACTERSINSCENE(KEY,SCENE_ID,CHAR_ID)` all characters in given scene, including PC's
    * `DAMAGEINSCENE(KEY,SCENE_ID,SRC_ID,TAR_ID,VAL)` doing combat per scene is the best way, I think
    * `CHARACTERINVENTORY(KEY,CHAR_ID,ITEM_ID)`
    * `ITEMS(ID,NAME,DESC)`

* ~~implement the rest of console functions~~
    * `give <char_id> <item_name> <item_desc>` should take whatever string as input for item name
    * `remove <item_id>`
    * `damage <int> <src_id> <tar_id>`  defaults as null
    * `heal <int> <src_id> <tar_id>`  defaults as null

* ~~change charinfo to display inventory~~

* append some columns to existing tables for more info
    * `CHARACTERS(BIO,INIT_MOD)`
    * `SCENES(DESC)`

* low priority console functions
    * `rollinit` roll initiative for all characters in scene
    * `advance` advance forward initiative

* when I have time
    * fix console input to take strings between quotation marks
    * add safeguards for sql queries
    * change sql tables to more sensible case