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
require_once("objects/indyobjects/indydataobjects.inc");

$OSCAILT_SCRIPT = "editredirects.php";
$textLabels = array("title" => "Manage URL Redirection",
	            "redirects" => "Redirects",
	            "object_id" => "Object Id",
	            "object_type" => "Object Type",
	            "primary_redirect_name" => "Primary Redirect Name",
	            "primary_redirect" => "Primary Redirect",
	            "other_redirect" => "Other Redirect Names",
	            "edit" => "Edit",
	            "delete" => "Delete",
	            "make_top" => "Make Top",
	            "for_object" => "for Object",
                    "no_filter_text" => "No Filter",
	            "object_deleted" => "object deleted!",
	            "CreateNewBtn" => "Create New Redirect",
	            "AutoGenBtn" => "Auto Generate &gt;&gt;",
	            "MoveTopBtn" => "Move To Top",
	            "edit_description" => "The primary redirect is the text string that will be offered to the user when they want to link to a page.",
	            "edit_primary_redirect" => "Primary Redirect",
	            "required" => "required",
	            "additional_psuedonyms" => "Additional Psuedonyms",
	            "additional_description" => "Additional synonyms allow you to map many different words to the same object. Enter a comma-separated list of additional synonyms for this object.",
	            "create_new_friendly_url" => "Create New Friendly URL for ",
	            "edit_friendly_url" => "Edit Friendly URL for ",
	            "are_you_sure_delete" => "Are you sure you wish to delete the friendly URL of ",
	            "delete_warning_msg" => "It is normally a very bad idea to delete URLs as any existing links that point at this URL will be broken!",
	            "primary_target_req" => "A Primary URL and a target ID are both required!",
	            "must_be_2alphachars_msg" => "Your primary URL must be at least 2 alphanumeric characters long ",
	            "create_invalid_target_id_msg" => "You have not specified a valid target ID to create the URL for",
	            "cannot_no_objects_config_msg" => "You can not create a friendly URL for this object as there are no page objects configured for it.",
	            "cancelBtn" => "&lt; &lt; Cancel",
	            "saveBtn" => "Save &gt;&gt;",
	            "deleteBtn" => "Delete &gt;&gt;");

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "editredirects") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Redirects. -Using defaults",""));
    $textObj->writeUserMessageBox();
} else {
    foreach (array_keys($textLabels) as $str_key ) {
	if (trim($textObj->getString($str_key)) != "" )
            $textLabels[$str_key] = $textObj->getString($str_key);
	else
        {
            if ($system_config->user_error_reporting == 8) $textLabels[$str_key] .= " using default ";
	} 
    }
}

addToPageTitle($textLabels["title"]);

function cleanseRedirect($txt)
{
   return preg_replace('/[^\w^\.]/', "", $txt);
}

function cleanseRedirects($txt)
{
   return preg_replace('/[^\w^\.^,]/', "", $txt);
}
function getDropdownList( $ObjectArray, $FilteredSelection, $no_initial=false)
{
   global $textLabels;

   if ($no_initial == true) $js_string = "";
   else $js_string = "<option value=\"No Filter\">".$textLabels['no_filter_text']."</option>";

   $form_string ="<option value=\"";
   foreach ( $ObjectArray as $each_type)
   {
       if ($each_type == $FilteredSelection)
       {
            $js_string .= "<option selected value=\"" . $each_type ."\">" . ucfirst($each_type) . "</option>";
       }
       else
       {
            $js_string .= $form_string . $each_type ."\">" . ucfirst($each_type) . "</option>";
       }
   }

   return $js_string;
}

function writeRedirectList()
{
   global $redirectList, $system_config, $userLanguage, $obj_man, $OSCAILT_SCRIPT;
   global $textLabels, $legalDataTypes;

   if(isset($_REQUEST['sort_by_type']) && $_REQUEST['sort_by_type']=="true" ) $sort_by_type_mode = "false";
   else $sort_by_type_mode = "true";

   if(isset($_REQUEST['sort_by_name']) && $_REQUEST['sort_by_name']=="true" ) $sort_by_name_mode = "false";
   else $sort_by_name_mode = "true";

   if(isset($_REQUEST['sort_by_id']) && $_REQUEST['sort_by_id']=="true" ) $sort_by_id_mode = "false";
   else $sort_by_id_mode = "true";

   $sort_message = " ( ". count($redirectList->redirects) ." )";

   if ( isset($_REQUEST['sort_by_name']) && $_REQUEST['sort_by_name']=="true" ) {
      $sort_message= " &nbsp; <small>sorted by primary name </small>";
   } elseif ( isset($_REQUEST['sort_by_type']) && $_REQUEST['sort_by_type']=="true" ) {
      $sort_message= " &nbsp; <small>sorted by object type </small>";
   } elseif ( isset($_REQUEST['sort_by_id']) && $_REQUEST['sort_by_id']=="true" ) {
      $sort_message= " &nbsp; <small>sorted by object id </small>";
   }


   $def_filter_object="No Filter";
   if ( isset($_REQUEST['filter_object'])) { $def_filter_object = $_REQUEST['filter_object'];}

   $js_string = getDropdownList(array_keys($legalDataTypes), $def_filter_object);

   // The form for the radio buttons to move items to the top

   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=8><?=$textLabels['redirects'] .$sort_message?></td>
   </tr>

   <form name="move_to_top" action="editredirects.php" method=post>

   <tr class=admin>
      <th class=admin>&nbsp;# &nbsp;</th>
      <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort_by_id=<?=$sort_by_id_mode?>"><?=$textLabels['object_id']?></a>&nbsp;</th>
      <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort_by_type=<?=$sort_by_type_mode?>"><?=$textLabels['object_type']?></a> &nbsp;</th>
      <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort_by_name=<?=$sort_by_name_mode?>"><?=$textLabels['primary_redirect_name']?></a>&nbsp;</th>

      <th class=admin>&nbsp;<?=$textLabels['other_redirect']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['edit']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['delete']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['make_top']?>&nbsp;</th>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;</td>
      <td class=admin><select name='filter_object' onchange=submit()><? echo $js_string;?></select></td>
      <td class=admin>&nbsp;</td>
      <td class=admin>&nbsp;</td>
      <td class=admin>&nbsp;</td>
      <td class=admin>&nbsp;</td>
      <td class=admin>&nbsp;</td>
      <td class=admin>&nbsp;</td>
   </tr>
   <?
   $redirected_ids = array();

   $rIndex = 0;
   $redirectsArray = array();
   $array_order = array();
   foreach($redirectList->redirects as $r)
   {
      $id = $r->id;
      if(isset($_REQUEST['sort_by_type']) && $_REQUEST['sort_by_type']=="true" )
      {
         $obj_typename = $obj_man->obj_set->getObjTypeName($id);
         if($obj_typename == "") $obj_typename = "deleted";
         $redirectsArray[] = $obj_typename;
      }
      elseif(isset($_REQUEST['sort_by_name']) && $_REQUEST['sort_by_name']=="true" )
      {
         $aliases = $r->names;
         if(!is_array($aliases) or count($aliases) == 0) $primary_alias = "none";
	 else $primary_alias = htmlspecialchars(array_shift($aliases), ENT_QUOTES);
         $redirectsArray[] = $primary_alias;
      }
      elseif(isset($_REQUEST['sort_by_id']) && $_REQUEST['sort_by_id']=="true" )
      {
         $redirectsArray[] = $id;
      }

      $array_order[$rIndex] = $id;
      $rIndex++;
   }
   // Sort by type or name. 
   if ( isset($_REQUEST['sort_by_name']) && $_REQUEST['sort_by_name']=="true" ) {
      array_multisort($redirectsArray, $array_order);
   } elseif ( isset($_REQUEST['sort_by_type']) && $_REQUEST['sort_by_type']=="true" ) {
      array_multisort($redirectsArray, $array_order);
   } elseif ( isset($_REQUEST['sort_by_id']) && $_REQUEST['sort_by_id']=="true" ) {
      array_multisort($redirectsArray, $array_order);
   }

   $counter = 1;
   // foreach($redirectList->redirects as $r)
   // Note: redirects in redirectList is organised by the redirect Id and not from 0 ... n
   for($rIndex=0;$rIndex < count($redirectList->redirects);$rIndex++)
   {
      $sorted_index=$array_order[$rIndex];
      $r=$redirectList->redirects[$sorted_index];
      $id = $r->id;
//echo("sorted index " .$sorted_index. " r id " .$id . "<BR>");
      $redirected_ids[] = $id;
      $obj_typename = ucfirst(strtolower($obj_man->obj_set->getObjTypeName($id)));
      if ( $def_filter_object != "No Filter") {
          if ($obj_typename != ucfirst($def_filter_object)) {
              $counter++;
              continue;
	  }
      }
      $ob_site=1;
      if(isset($obj_man->obj_set->itemCollection[$id])) {
          $ob_site = $obj_man->obj_set->itemCollection[$id]->site_id;
      }
      if($obj_typename != "") $obj_typename = "<a href='admin.php?action=list&site_id=".$ob_site."&obj_type=".$obj_typename."'>".$obj_typename."</a>";
	 
      if($obj_typename == "") $obj_typename = "<span class='error'>".$textLabels['object_deleted']."</span>";
      $aliases = $r->names;
      if(!is_array($aliases) or count($aliases) == 0) continue;
      $primary_alias = htmlspecialchars(array_shift($aliases), ENT_QUOTES);

      // Turn primary alias into URL
      $primary_alias = "<a href='http://".$system_config->site_url."/".$primary_alias."'>".$primary_alias."</a>";

      if(count($aliases) > 0)
      {
         $alias_str = htmlspecialchars(implode(", ", $aliases), ENT_QUOTES);
      }
      else $alias_str = "";

      $rowspan = 1;
      // If the type is DocumentRedirector, then show the level2 directory entries if any.
      if($r->type == "DocumentRedirector")
      {
          $level2_str = "";
	  $l2_cnt =0;
          if($r->level2_entries != null)
          {
              foreach($r->level2_entries as $lv2_obj_name => $lv2_obj_id) {
	          if ($l2_cnt > 0 ) $level2_str .= "<br>";
                  $level2_str .= " &nbsp; ". $primary_alias ."/".$lv2_obj_name . " &nbsp; (". $lv2_obj_id.")";
	          $l2_cnt++;
              }
              $rowspan = 2;
          }
      }

      $checked_str = "";
      if ($counter == 1 ) $checked_str = "checked";
      ?>
      <tr class=admin>
         <td class=admin align=center rowspan=<?=$rowspan?>>&nbsp;<?=$counter?>&nbsp;</td>
         <td class=admin align=center rowspan=<?=$rowspan?>>&nbsp;<?=$id?>&nbsp;</td>
         <td class=admin align=center >&nbsp;<?=$obj_typename?>&nbsp;</td>
         <td class=admin align=center><?=$primary_alias?></td>
         <td class=admin align=center>&nbsp;<?=$alias_str?>&nbsp;</td>
         <td class=admin align=center rowspan=<?=$rowspan?>><a href="editredirects.php?subpage=edit&target_id=<?=$id?>"><img src='graphics/edit.gif' border=0></a></td>
         <td class=admin align=center rowspan=<?=$rowspan?>><a href="editredirects.php?subpage=delete&target_id=<?=$id?>"><img src='graphics/delete.gif' border=0></a></td>
         <td class=admin align=center rowspan=<?=$rowspan?>><input type=radio name="move_to_top" value="<?=$id?>" <?=$checked_str?>></td>
      </tr>
      <?
      // If the type is DocumentRedirector, then show the level2 directory entries if any.
      if($r->type == "DocumentRedirector")
      {
          if($level2_str != "")
          {
	      ?>
              <tr class=admin>
                 <td class=admin align=left colspan=2>Level 2 Directory Entries</td>
                 <td class=admin align=left><?=$level2_str?></td>
              </tr>
              <?
          }
      }

      $counter++;
   }
   ?>
      <tr class=admin>
       <td colspan=8 align=right>
       <input type=submit name="move_swap" value="<?=$textLabels['MoveTopBtn']?>">
       </td>
      </tr>
       </form>
   <?

   // Now get all the object ids created and if any of them are not in the lis of redirects, then generate
   // an option for the user to auto generate a redirect for that object id, so long as it is not of type
   // site or feed export.
   $available_redirects = $obj_man->obj_set->getAllObjectIDs();
   $options = array();
   foreach($available_redirects as $red)
   {
      if(!in_array($red, $redirected_ids))
      {
         $obj = $obj_man->obj_set->fetchObject($red, $userLanguage->getLanguageCodePrefix());
         if($obj == false) continue;
         if(($obj->getPageLayoutRef() === false or strtolower($obj->getType()) == 'site') && $obj->getType() != 'FEEDEXPORT') continue;

         $options[] = "<option value='$red'>$obj->objref ".htmlspecialchars($obj->name()." (".strtolower($obj->getType()), ENT_QUOTES).")</option>";
      }
   }
   if(count($options) > 0)
   {
      $option_str = implode("\n", $options);
   ?>
      <tr class=admin>
         <td colspan=8 align=center>
         <form name="manual_generate_url" action="editredirects.php" method=post style='display:inline'>
         <input type=hidden name=subpage value="edit">
	 <input type=submit name="create" value="<?=$textLabels['CreateNewBtn']?>"> <?=$textLabels['for_object']?>: <select name="target_id"><?=$option_str?></select>
         </form> or
         <form  name="auto_generate_url" action="editredirects.php" method=post style='display:inline'>
         <input type=hidden name=subpage value="generate">
	 <input type=submit name="create" value="<?=$textLabels['AutoGenBtn']?>">
         </form>
         </td>
      </tr>
   <?
   }
   ?>
   </table>
   <?
}

function generateFriendlyURLs()
{
   global $redirectList, $system_config, $obj_man, $legalDataTypes,$userLanguage;
   $available_redirects = $obj_man->obj_set->getAllObjectIDs();
   foreach($available_redirects as $red_id)
   {
      $red = $redirectList->getRedirect($red_id);
      if($red !== false) continue;
      $obj = $obj_man->obj_set->fetchObject($red_id, $userLanguage->getLanguageCodePrefix());
      if($obj == false) continue;
      if($obj->getPageLayoutRef() === false or strtolower($obj->getType()) == 'site') continue;
      $nm = $obj->name();
      $nm = ereg_replace("[^a-z0-9._]", "", str_replace(" ", "_", strtolower(trim($nm))));
      if($nm == "") $nm = 'unnamed';
      if($redirectList->containsRedirectString($nm))
      {
         $i = 1;
         while($redirectList->containsRedirectString($nm.'_'.$i))
         {
            $i++;
         }
         $nm = $nm.'_'.$i;
      }
      $type = strtolower($obj->getType());
      if($type == 'article') $redirect = new ArticleRedirector($red_id, array($nm));
      elseif($type == 'feedexport') $redirect = new FeedRedirector($red_id, array($nm));
      elseif($type == 'document') $redirect = new DocumentRedirector($red_id, array($nm));
      elseif(isset($legalDataTypes[$type]) && $legalDataTypes[$type] == 'module') $redirect = new FilteredRedirector($red_id, array($nm));
      else $redirect = new Redirector($red_id, array($nm));
      $redirectList->redirects[] = $redirect;
   }
   $redirectList->save();
}

function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function writeEditBox()
{
   global $system_config, $userLanguage, $obj_man, $redirectList;
   global $textLabels;

   $target_id = cleanseNumericalQueryField($_REQUEST['target_id']);
   if($target_id <= 0)
   {
      writeRedirectList();
      return;
   }
   $obj = $obj_man->obj_set->fetchObject($target_id, $userLanguage->getLanguageCodePrefix());
   $type = strtolower($obj->getType());
   if($obj !== false)
   {
      $name = $obj->name();
   }
   else
   {
      writeFailureMessage("Can't Edit Deleted Object", "The object with id $target_id has been deleted.  You must delelete this friendly url before you can re-assign the url to another object");
      return;
   }
   $redirect = $redirectList->getRedirect($target_id);

   // New code to automatically convert any document object that is still just a Redirector to DocumentRedirector
   // The approach is to create a new object and delete the old.
   if($type == 'document') {
      if ($redirect->type == "Redirector") {
          $new_redirect = new DocumentRedirector($target_id, $redirect->names);
          // Now set the old one to null and set pointer in array to the new
          $redirect = null;
          $redirect = &$new_redirect;
          $redirectList->redirects[$target_id] = $redirect;
       }
   }


   
   if(isset($_REQUEST['primary_redirect']))
   {
      $primary_filling = cleanseRedirect($_REQUEST['primary_redirect']);
   }
   elseif($redirect !== false)
   {
      $primary_filling = $redirect->getPrimaryRedirect();
   }
   else
   {
      $primary_filling = "";
   }

   if(isset($_REQUEST['secondary_redirects']))
   {
      $secondary_filling = cleanseRedirects($_REQUEST['secondary_redirects']);
   }
   elseif($redirect !== false)
   {
      $syns =  $redirect->getAdditionalSynonyms();
      if ($syns === false) $secondary_filling = "";
      else
      {
         if (count($syns) >= 1) $secondary_filling = implode(",", $syns);
         else $secondary_filling = "";
      }
   }
   else
   {
      $secondary_filling = "";
   }


   ?>
   <table class=admin align=center width=300 border=0>
   <form name="edit_url" action="editredirects.php" method=post>
   <input type=hidden name=subpage value="edit">
   <input type=hidden name="target_id" value="<?=$target_id?>">
   <tr class=admin>
      <th class=admin colspan=4>
      <?
      if(isset($_REQUEST['create'])) echo($textLabels['create_new_friendly_url'] .$name);
      else echo($textLabels['edit_friendly_url'].$name);
      ?>
      </th>
   </tr>
   <tr class=admin>
   <td class=admin colspan=4> <?=$textLabels['edit_description']?>
      <!--The primary redirect is the text string that will be offered to the user when they want to link to a page.-->
      </td>
   </tr>
   <tr class=admin>
   <td class=admin>&nbsp;<B><?=$textLabels['object_id']?>:</B> </td>
      <td class=admin colspan=3><?=$target_id?></td>
   </tr>
   <tr class=admin>
   <td class=admin>&nbsp;<B><?=$textLabels['primary_redirect']?>:</B> <small class='error'>(<?=$textLabels['required']?>)</small>&nbsp;</td>
      <td class=admin colspan=3><input name="primary_redirect" value="<?=htmlspecialchars($primary_filling, ENT_QUOTES)?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4> <?=$textLabels['additional_description']?>
      <!--Additional synonyms allow you to map many different words to the same object. Enter a comma-separated list of additional synonyms for this object.-->
      </td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;<B><?=$textLabels['additional_psuedonyms']?>:</B>&nbsp;</td>
      <td class=admin colspan=3><textarea rows="3" cols="20" name="secondary_redirects"><?=htmlspecialchars($secondary_filling, ENT_QUOTES)?></textarea></td>
   </tr>
   <?
   // For objects of type document add in the code here for subdirectory synonyms.
   if($type == 'document') {
      ?>
      <tr class=admin>
      <td class=admin colspan=4>&nbsp;<B>Second Level Directory Object Links:</B>&nbsp;</td>
      </tr>
      <?
      $level2_totals_selected =0;
      // If this is a DocumentRedirector show if any, the 2nd level directory entries.
      if ($redirect->type == "DocumentRedirector") {
      
          $obj_man->obj_set->object_name_cache->load(array("DOCUMENT"));
          ?>
          <tr class=admin>
             <th class=admin colspan=1>Redirect / Object Name</th>
             <th class=admin colspan=2>Object Id</th>
             <th class=admin align=center>Select</th>
          </tr>
          <?
          // Add a list of choices of other Doc redirects for user to pick.
          foreach($redirect->level2_entries as $sub_dir => $lvl2_obj_id)
          {
              $level2_totals_selected++;
	      $data_value = $lvl2_obj_id."::" . $sub_dir;
              ?>
              <tr class=admin>
              <?
              if ( isset($redirectList->redirects[$lvl2_obj_id]) ) {
                   ?> <td class=admin colspan=1><?=$sub_dir?></td> <?
              } else {
                   ?> <td class=admin colspan=1>No redirect for this object: <?=$sub_dir?></td> <?
              }
              ?>
                   <td class=admin colspan=2><?=$lvl2_obj_id?></td>
                   <td class=admin align=center><input type="checkbox" name="dir_level2_<?=$level2_totals_selected?>" checked>
                   <input type="hidden" name="level2_data_<?=$level2_totals_selected?>" value="<?=$data_value?>">
		   </td>
              </tr>
              <?
          }
          //$all_objects = $obj_man->obj_set->getAllObjectIDs();
          $all_objects = $obj_man->obj_set->getObjectStubs();
          foreach($all_objects as $obj_stub)
          {
              if ($obj_stub->obj_type == "DOCUMENT" && $obj_stub->obj_id != $target_id) {
                  if ($redirect->level2_entries != null)
                  {
                      if (in_array($obj_stub->obj_id, array_values($redirect->level2_entries))) continue;
                  }

                  $level2_totals_selected++;
                  ?>
                  <tr class=admin>
                      <?
                      if ( isset($redirectList->redirects[$obj_stub->obj_id]) ) {
                          ?>
                          <td class=admin><?=$redirectList->redirects[$obj_stub->obj_id]->getPrimaryRedirect()?></td>
                          <?
	                  $data_value = $obj_stub->obj_id."::" . $redirectList->redirects[$obj_stub->obj_id]->getPrimaryRedirect();
                      } else {
                          $tmp_ob = $obj_man->obj_set->object_name_cache->getObjectName("document", $obj_stub->obj_id, $obj_stub->primary_language_code, $obj_stub->primary_language_code);
	                  $data_value = $obj_stub->obj_id."::" . $tmp_ob; 
                          ?>
                          <td class=admin>No redirect for this object: <?=$tmp_ob?></td>
                          <?
                      }
                      ?>
                      <td class=admin colspan=2><?=$obj_stub->obj_id?></td>
                      <td class=admin align=center>
		      <input type="checkbox" name="dir_level2_<?=$level2_totals_selected?>">
                      <input type="hidden" name="level2_data_<?=$level2_totals_selected?>" value="<?=$data_value?>">
                      </td>
                  </tr>
                  <?
              }
          }
      }
      ?>
      <tr>
      <td colspan=4 align=center>
      <input type=hidden name=dir_level2_total value="<?=$level2_totals_selected?>">
      </td>
      </tr>
      <?
   }
   ?>
   <tr>
      <td colspan=4 align=center>
      <input type=submit name=cancel value="<?=$textLabels['cancelBtn']?>">
      <input type=submit name=save value="<?=$textLabels['saveBtn']?>">
      </td>
   </tr>
   </form>
   </table>
   <?
}

function loadManagedObjectSet()
{
   global $system_config, $OSCAILT_SCRIPT, $obj_man;
   $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $OSCAILT_SCRIPT);
   $obj_man->obj_set = new indyObjectSet($obj_man->type_dir, $obj_man->storage);
   $sites = array("*");
   $types = array("*");
   if(!$obj_man->obj_set->load($sites, $types, $obj_man->action_req))
   {
      writeError( "Programme Error: Failed to Load Set of Managed Objects: Updates Not Saved");
      return false;
   }
   return true;
}




function writeConfirmDeleteBox()
{
   global $system_config, $userLanguage, $obj_man, $redirectList, $editor_session;
   global $textLabels;

   $target_id = cleanseNumericalQueryField($_REQUEST['target_id']);
   if($target_id <= 0)
   {
      writeRedirectList();
      return;
   }
   $obj = $obj_man->obj_set->fetchObject($target_id, $userLanguage->getLanguageCodePrefix());
   if(!$obj)
   {
      $redirectList->removeRedirect($target_id);
      if($editor_session->editor->allowedWriteAccessTo("editredirects"))
      {
         $redirectList->save();
         $redirectList->load(true);
      }
      else
      {
         $editor_session->writeNoWritePermissionError();
      }
      writeRedirectList();
      return;
   }
   ?>
   <table align=center>
   <form name="cnf_delete" action="editredirects.php" method="post">
   <input type="hidden" name="subpage" value="delete">
   <input type="hidden" name="target_id" value="<?=$target_id?>"><?
   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B><?=$textLabels['are_you_sure_delete']?> <?=htmlspecialchars(strtolower($obj->getType()), ENT_QUOTES)?> <?=$target_id?>, <i><?=htmlspecialchars($obj->name(), ENT_QUOTES)?></i></B><BR><BR>
      <?=$textLabels['delete_warning_msg']?>
      <!--It is normally a very bad idea to delete URLs as any existing links that point at this URL will be broken!-->
      </td>
   </tr>
   <tr>
   <td align=right><input type=submit name=cancel value="<?=$textLabels['cancelBtn']?>"></td>
   <td><input type=submit name=confirm value="<?=$textLabels['deleteBtn']?>"></td>
   </tr>
   </form>
   </table>
   <?
}

function createRedirectFromForm()
{
   global $system_config, $userLanguage, $obj_man, $redirectList, $legalDataTypes;
   global $textLabels;

   if(!isset($_REQUEST['primary_redirect']) or !isset($_REQUEST['target_id']))
   {
      // writeError("A Primary URL and a target ID are both required");
      writeError($textLabels['primary_target_req']);
      return false;
   }
   $primary = cleanseRedirect($_REQUEST['primary_redirect']);
   if(strlen($primary) <= 1)
   {
      // writeError("Your primary URL must be at least 2 alphanumeric characters long $primary");
      writeError($textLabels['must_be_2alphachars_msg']);
      return false;
   }
   $synonyms = array();
   $synonyms[] = $primary;
   $secondaries = cleanseRedirects($_REQUEST['secondary_redirects']);
   $all_secondaries = explode(",", $secondaries);
   foreach($all_secondaries as $s)
   {
      if(strlen($s) > 1)
      {
         $synonyms[] = $s;
      }

   }

   $target_id = cleanseNumericalQueryField($_REQUEST['target_id']);
   if($target_id <= 0)
   {
      // writeError("You have not specified a valid target ID to create the URL for");
      writeError($textLabels['create_invalid_target_id_msg']);
      return false;
   }
   $obj = $obj_man->obj_set->fetchObject($target_id, $userLanguage->getLanguageCodePrefix());
   if($obj->getPageLayoutRef() === false && $obj->getType() != 'FEEDEXPORT')
   {
      // writeError("You can not create a friendly URL for this object as there are no page objects configured for it.");
      writeError($textLabels['cannot_no_objects_config_msg']);
      return false;
   }

   $type = strtolower($obj->getType());

   // Now for document type, read the form data for the level 2 directory entries.
   $level2_array = array();
   if($type == 'document') {
       $total_vars = 0;
       if (isset($_REQUEST['dir_level2_total'])) $total_vars = $_REQUEST['dir_level2_total'];
       for ($ivar = 1; $ivar <= $total_vars; $ivar++)
       {
           if (isset($_REQUEST['dir_level2_'.$ivar]) && $_REQUEST['dir_level2_'.$ivar] == "on")
	   {
               if (isset($_REQUEST['level2_data_'.$ivar]) )
                   list($form_obj_id, $form_obj_name) = explode("::", $_REQUEST['level2_data_'.$ivar]);
                   $level2_array[$form_obj_name] = $form_obj_id;
	   }
       }
   }

   if($type == 'article') $redirect = new ArticleRedirector($target_id, $synonyms);
   elseif($type == 'document') $redirect = new DocumentRedirector($target_id, $synonyms, $level2_array);
   elseif($type == 'feedexport') $redirect = new FeedRedirector($target_id, $synonyms);
   elseif(isset($legalDataTypes[$type]) && $legalDataTypes[$type] == 'module') $redirect = new FilteredRedirector($target_id, $synonyms);
   else $redirect = new Redirector($target_id, $synonyms);
   return $redirect;
}


ob_start();
$obj_man = null;
global $redirectList;
$redirectList->load();
loadManagedObjectSet();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("viewobjects.php","Object Usage");
   if($editor_session->editor->allowedReadAccessTo("editredirects"))
   {
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']))
      {
         $target_id = cleanseNumericalQueryField($_REQUEST['target_id']);
         if($target_id <= 0)
         {
            writeRedirectList();
            return;
         }

         $redirectList->removeRedirect($target_id);
         if($editor_session->editor->allowedWriteAccessTo("editredirects"))
         {
            $redirectList->save();
            $redirectList->load(true);
            writeRedirectList();
         }
         else
         {
            $editor_session->writeNoWritePermissionError();
            writeEditBox();
         }
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage'] =="delete" && isset($_REQUEST['cancel']))
      {
         writeRedirectList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
      {
         writeConfirmDeleteBox();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']))
      {
         $redirect = createRedirectFromForm();
         if($redirect === false)
         {
            writeEditBox();
         }
         else
         {
            $redirectList->removeRedirect($redirect->id);
            $redirectList->redirects[] = $redirect;

            if($editor_session->editor->allowedWriteAccessTo("editredirects"))
            {
               $redirectList->save();
               $redirectList->load(true);
               writeRedirectList();
            }
            else
            {
               $editor_session->writeNoWritePermissionError();
               writeEditBox();
            }

         }
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && !isset($_REQUEST['cancel']))
      {
         writeEditBox();
      }
      elseif(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="generate")
      {
         generateFriendlyURLs();
         writeRedirectList();
      }
      elseif(isset($_REQUEST['move_to_top']) )
      {
         if ($redirectList->getRedirect($_REQUEST['move_to_top']) === false) writeRedirectList();
	 else
	 {
	     $switch_redirect = $redirectList->getRedirect($_REQUEST['move_to_top']);
             $redirectList->removeRedirect($_REQUEST['move_to_top']);
	     array_unshift($redirectList->redirects, $switch_redirect);
             $redirectList->save();
             $redirectList->load(true);
             writeRedirectList();
         }
      }
      else
      {
         writeRedirectList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?> 
