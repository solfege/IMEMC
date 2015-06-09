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
$OSCAILT_SCRIPT = "editexport.php";
require_once("objects/indyobjects/indydataobjects.inc");
require_once("objects/indyobjects/indysiteimporter.inc");
require_once("objects/indyobjects/indysiteexporter.inc");
require_once("objects/adminutilities.inc");



function writeExportIntroForm()
{
   global $system_config, $editor_session, $txt_strs, $obj_set, $exporter;
   $cols = 4;
   $new_txt = $txt_strs->getString("create_export_text");
   if($editor_session->editor->allowedWriteAccessTo("exportdataobjects"))
   {
      $title = $txt_strs->getString("export_title");
      $intro = $txt_strs->getString("export_intro");
      echo "<h3>$title</h3><P>$intro</P>";
      echo "<b><a href='?action=create'>";
      echo $new_txt;
      echo "</a></b>";
      echo "<hr>";
      $title = $txt_strs->getString("view_exports_title");
      $intro = $txt_strs->getString("view_exports_intro");
      echo "<h3>$title</h3><P>$intro</P>";
      $exporter->writeChooseCollectionSection($cols, $txt_strs);
   }
   elseif($editor_session->editor->allowedReadAccessTo("exportdataobjects"))
   {
      $title = $txt_strs->getString("view_exports_title");
      $intro = $txt_strs->getString("view_exports_intro");
      echo "<h3>$title</h3><P>$intro</P>";
      $exporter->writeChooseCollectionSection($cols, $txt_strs);
   }
}


function writeNewExportForm()
{
   global $exporter;
   $cols = 2;
   writeMenuSpacer();
   writeMenuHeader();
   echo "<form name='exportform' action='' method=post>\n";
   echo "<input type='hidden' name='action' value='create'>";
   $exporter->writeExportForm($cols, true);
   echo "</form>";
   writeMenuFooter($cols);
}

function writeViewExportPage()
{
   if(!isset($_REQUEST['target_collection']))
   {
      writeError("No target collection specified");
   }
   else
   {
      global $exporter;
      $cols = 2;
      writeMenuSpacer();
      writeMenuHeader();
      echo "<form name='exportform' action='' method=post>\n";
      $exporter->writeExportForm($cols, false, $_REQUEST['target_collection']);
      echo "</form>";
   }

}


function writeExportPage()
{
   if(!isset($_REQUEST['target_collection']))
   {
      writeError("No target collection specified");
   }
   else
   {
      global $exporter;
      $cols = 2;
      writeMenuSpacer();
      writeMenuHeader();
      echo "<form name='editobjectform' action='' method=post>\n";
      $exporter->writeChoiceForm($cols, $_REQUEST['target_collection']);
      echo "</form>";
   }
}

function writeConfirmDeletePage()
{
   echo "<P>Confirm Delete</P>";
}

function writeConfirmExportPage()
{
   global $exporter;
   echo "<form name='exportform' action='' method=post>\n";
   echo "<input type='hidden' name='action' value='create'>";
   echo "<input type='hidden' name='save' value='true'>";
   $exporter->writeHiddenChoices();
   $warning = "Check to make sure you want to export the data below";
   $buttons = "<p>".$exporter->getChoicesInfo();
   $act = "Export Data To File";
   $title = "Confirm $act";
   $buttons .= "<input type='submit' name='confirm' value='Confirm Export &gt;&gt;'>";
   $id = "";
   $reqd = true;
   echo $exporter->getNotifyForm($title, $warning, $act, "", $id, $reqd, $buttons);
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
$exporter = new indySiteExporter($obj_set, $dummy);

if(!$e && $editor_session->isSessionOpen())
{
   writeAdminHeader();
   if($editor_session->editor->allowedReadAccessTo("exportdataobjects") )
   {
      if(isset($_REQUEST['action']))
      {
        $action = $_REQUEST['action'];
      }
      else
      {
         $action = '';
      }

      if(isset($_REQUEST['save']))
      {
         if(isset($_REQUEST['confirm']))
         {
            if($action == 'create')
            {
               if($editor_session->editor->allowedWriteAccessTo("exportdataobjects"))
               {
                  $exporter->readChoices();
                  if($exporter->checkUserInput())
                  {
                     if($exporter->export())
                     {
                         $exporter->writeUserMessageBox();
                         writeError("Successfully exported to file");
                         writeExportIntroForm();
                     }
                  }
                  else
                  {
                     writeError($exporter->getUserMessage());
                     writeNewExportForm();
                  }
               }
               else
               {
                  $editor_session->writeNoWritePermissionError();
               }
            }
            elseif($action == 'edit')
            {
               echo "<P>not going to allow this I think</P>";
            }
            elseif($action == 'delete')
            {
               echo "<P>Do I really want to allow deletion?</P>";
            }
         }
         else
         {
            //perform checks either write out form or write confirm page
            if($action == 'create')
            {
               if($editor_session->editor->allowedWriteAccessTo("exportdataobjects"))
               {
                  $exporter->readChoices();
                  if($exporter->checkUserInput())
                  {
                     writeConfirmExportPage();
                  }
                  else
                  {
                     writeError($exporter->getUserMessage());
                     writeNewExportForm();
                  }
               }
               else
               {
                  $editor_session->writeNoWritePermissionError();
               }
            }
            elseif($action == 'edit')
            {
                echo "<P>not going to allow this I think</P>";
            }
         }
      }
      else
      {
         if($action == 'create')
         {
            writeNewExportForm();
         }
         elseif($action == 'view')
         {
            writeViewExportPage();
         }
         elseif($action == 'edit')
         {
            writeExportPage();
         }
         elseif($action == 'delete')
         {
            writeConfirmDeletePage();
         }
         else
         {
            writeExportIntroForm();
         }
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


include_once("adminfooter.inc");

?> 