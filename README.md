#dmagec
A C++ console application for managing a tabletop role-playing game.

###database tables

    CHARACTERS(ID,NAME,REALNAME,CURHP,TOTALHP,XP)
    SCENES(ID,NAME)

###stuff to do
1. ~~implement the rest of console functions not reliant on more database tables~~
    * ~~easy~~
        * `sceneinfo`
    * ~~a little harder~~
        * `editscene`
        * `editchar`

* implement more database tables
    * `CHARACTERSINSCENE(SCENE_ID,CHAR_ID)` all characters in given scene, including PC's
    * `DAMAGEINSCENE(SCENE_ID,SRC_ID,TAR_ID)` doing combat per scene is the best way, I think
    * `CHARACTERINVENTORY(CHAR_ID,ITEM_NAME)` maybe should have seperate table for all items? more stats available!

* implement the rest of console functions
    * `give <id> <item>` should take whatever string as input for item name
    * `remove <id> <item>`
    * `damage <int> <src_id> <tar_id>`  defaults as null
    * `heal <int> <src_id> <tar_id>`  defaults as null

* append some columns to existing tables for more info
    * `CHARACTERS(BIO,INIT_MOD)`
    * `SCENES(INFO)`

* low priority console functions
    * `rollinit` roll initiative for all characters in scene
    * `advance` advance forward initiative

* when I have time
    * fix console input to take strings between quotation marks
    * add safeguards for sql queries