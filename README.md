#dmagec
A C++ console application for managing a tabletop role-playing game.

###database tables

    Characters(ID INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, realName text, curHP int, totalHP int, xp int, bio text, init_mod int);
    Scenes(ID INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, description text);
    Items(ID INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, description text);
    CharacterInventory(ID INT PRIMARY KEY NOT NULL, char_id INT NOT NULL, item_id INT NOT NULL);
    DamageInScene(ID INT PRIMARY KEY NOT NULL, scene_id INT NOT NULL, val INT NOT NULL, tar_id INT NOT NULL, src_id INT);
    CharactersInScene(Key INT PRIMARY KEY NOT NULL, scene_id INT NOT NULL, char_id INT NOT NULL);

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

* ~~append some columns to existing tables for more info~~
    * `CHARACTERS(BIO,INIT_MOD)`
    * `SCENES(DESC)`

* low priority console functions
    * `rollinit` roll initiative for all characters in scene
    * `advance` advance forward initiative
    * `sceneNPCs` output a list of NPCs in scene

* need to do these to make program work properly
    * ~~add function to show npcs in scene~~
    * fix console input to take strings between quotation marks
    * `addscene`
    * add functions for CharactersInScene, add/remove etc

* when I have time
    * add safeguards for sql queries
    * ~~change sql tables to more sensible case~~
    * ~~fix multiscreen:act() to be better~~ did this but I think we can rid the function entirely and just call functions directly
    * initialize a scene if none exists, program crashes without one
    * make inventory display in character info better
    * improve formatting for sceneinfo
    * show an alert with character names with damage/heal functions
    * get rid of getChars() function and do the SQL queries in individual functions instead
    * make it so charlist can scroll or something