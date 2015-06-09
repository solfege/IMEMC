<?php
//Actions on content items are divided into 5 groups for notifications - from none to all 

/*just here fyi
$all_item_actions = array(
   "attachment" => array('clip', 'unclip', 'copy', "copyandunhide", "create",
               "move", "edit", "hide", "unhide", "delete", "featurize", "unfeaturize"),
   "comment" => array('clip', 'unclip', 'upgrade', 'copy', "move", "edit", "hide", "unhide", "delete"),
   "story" => array('clip', 'unclip', 'downgrade', "upgrade", "edit", "hide", "unhide", "lock", "unlock", "delete", "stick", "unstick"),
   "feature" => array('clip', 'unclip', "edit", "hide", "unhide", "lock", "unlock", "delete", "stick", "unstick")
);

//never generate a notify
$always_item_actions = array(
   "attachment" => array('clip', 'unclip'),
   "comment" => array('clip', 'unclip', 'release'),
   "story" => array('clip', 'unclip', 'release'),
   "feature" => array('clip', 'unclip', 'release'),
)

//only generate a notify if level is all
$alllevel_item_actions = array(
   "attachment" => array('copy', "copyandunhide", "create", "featurize"),
   "comment" => array('copy')
);
*/


//if level set to high
$highlevel_item_actions = array(
   "attachment" => array("move", "edit", "unhide"),
   "comment" => array("move", 'upgrade', "unhide"),
   "story" => array('downgrade', "unhide"),
);

//if level set to medium
$mediumlevel_item_actions = array(
   "attachment" => array("hide"),
   "comment" => array("edit", "hide", "delete"),
   "story" => array("edit", "hide", "lock", "unlock", "stick", "unstick"),
   "feature" => array("edit", "hide", "lock", "unlock", "stick", "unstick")
);

//if level set to low
$lowlevel_item_actions = array(
   "attachment" => array("delete", "unfeaturize"),
   "comment" => array("delete"),
   "story" => array("upgrade", "delete"),
   "feature" => array("unhide", "delete")
);
?>