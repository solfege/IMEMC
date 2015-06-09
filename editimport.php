<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/
require_once("oscailt_init.inc");
addToPageTitle("Oscailt Import / Export Page");
$OSCAILT_SCRIPT = "editimport.php";
require_once("objects/indyobjects/indydataobjects.inc");
require_once("objects/indyobjects/indysiteimporter.inc");
require_once("objects/indyobjects/indysiteexporter.inc");
require_once("objects/adminutilities.inc");



function writeImportIntroPage()
{
   global $system_config, $editor_session, $txt_strs, $obj_set, $importer;
   $cols = 4;
   $new_txt = $txt_strs->getString("create_export_text");
   if($editor_session->editor->allowedWriteAccessTo("importdataobjects"))
   {
      $title = $txt_strs->getString("import_title");
      $intro = $txt_strs->getString("import_intro");
      echo "<h3>$title</h3><P>$intro</P>";
      $importer->writeChooseCollectionSection($cols, $txt_strs, "import");
   }
   elseif($editor_session->editor->allowedReadAccessTo("importdataobjects"))
   {
      $title = $txt_strs->getString("view_imports_title");
      $intro = $txt_strs->getString("view_imports_intro");
      echo "<h3>$title</h3><P>$intro</P>";
      $importer->writeChooseCollectionSection($cols-1, $txt_strs, "import");
   }
}

function writeChooseImportPage()
{
   if(!isset($_REQUEST['target_collection']))
   {
      writeError("No target collection specified");
   }
   else
   {
      global $importer, $txt_strs;
      $cols = 2;
      writeMenuSpacer();
      writeMenuHeader();
      echo "<form name='importform' action='?' method='POST'>\n";
      $importer->writeChoiceForm($cols, $txt_strs, $_REQUEST['target_collection'], true);
      echo "</form>";
   }
}


function writeImportPage()
{
   if(!isset($_REQUEST['target_collection']))
   {
      writeError("No target collection specified");
   }
   else
   {
      global $importer, $txt_strs;
      $cols = 3;
      echo "<form name='importform' action='?' method='GET'>\n";
      $importer->writeChoiceForm($cols, $txt_strs, $_REQUEST['target_collection']);
      echo "</form>";
   }
}

function writeConfirmImportPage()
{
   global $importer;
   echo "<form name='exportform' action='' method=post>\n";
   echo "<input type='hidden' name='action' value='".htmlspecialchars($_REQUEST['action'], ENT_QUOTES)."'>";
   echo "<input type='hidden' name='save' value='true'>";
   $importer->writeHiddenChoices();
   $warning = "Check to make sure you want to import the data below";
   $buttons = "<p>".$importer->getChoicesInfo();
   $act = "Import Data";
   $title = "Confirm $act";
   $buttons .= "<input type='submit' name='confirm' value='Confirm Import &gt;&gt;'>";
   $id = "";
   $reqd = true;
   echo $importer->getNotifyForm($title, $warning, $act, "", $id, $reqd, $buttons);
   echo "</form>";
}



ob_start();
$e = false;
$txt_strs = new indyItemSet();
if($txt_strs->load($system_config->xml_store, "universal_config_options") === false)
{
   writeError("Failed to load the list of XML Options from $system_config->xml_store");
   $e = true;
}
$obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
$dummy = new indyObjectActionRequest();
if(!$obj_set->load(array("*"), array("*"), $dummy))
{
   writeError("Failed to load the object set with data types from $system_config->xmltypedef_dir");
   $e = true;
}
$importer = new indySiteImporter($obj_set, $dummy);

if(!$e && $editor_session->isSessionOpen())
{
   writeAdminHeader();
   if($editor_session->editor->allowedReadAccessTo("importdataobjects"))
   {
      if(isset($_REQUEST['action']))
      {
        $action = $_REQUEST['action'];
      }
      else
      {
         $action = '';
      }
      if($action != 'import' && $action != 'import-choose')
      {
         writeImportIntroPage();
      }
      elseif(!isset($_REQUEST['save']))
      {
         if($action == 'import-choose')
         {
            writeChooseImportPage();
         }
         else
         {
            writeImportPage();
         }
      }
      elseif($editor_session->editor->allowedWriteAccessTo("importdataobjects"))
      {
         if($action == 'import')
         {
            $importer->readBasicChoices();
         }
         else
         {
            $importer->readFullChoices();
         }
         if($importer->checkUserInput())
         {
            if(isset($_REQUEST['confirm']))
            {
               if($importer->import($action, true))
               {
                  echo "<div class='user-message'>";
                  echo "<ul>";
                  echo "<li>";
                  echo implode("</li><li>", $importer->import_record);
                  echo "</ul></div>";
                  if(isset($_REQUEST['target_collection']))
                      logAction("", $_REQUEST['target_collection'], "objects", "import ok"); 
               }
               else
               {
                  writeError("Errors occurred during import: ".$importer->getUserMessage());
                  if(isset($_REQUEST['target_collection']))
                      logAction("", $_REQUEST['target_collection'], "objects", "import failure"); 
               }
               writeImportIntroPage();
            }
            else
            {
               writeConfirmImportPage();
            }
         }
         else
         {
            writeError($importer->getUserMessage());
            if($action == 'import')
            {
               writeImportPage();
            }
            else
            {
               writeChooseImportPage();
            }
         }
      }
      else $editor_session->writeNoWritePermissionError();
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


include_once("adminfooter.inc");

?>