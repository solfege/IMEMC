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
/* Description:                                                         */
/* This file displays basic information about the objects like their    */
/* ids, type, oscailt class, name, cache filename. It can sort by and   */
/* filter by various fields.                                            */
/************************************************************************/
require_once("oscailt_init.inc");
require_once("objects/indyobjects/indydataobjects.inc");
$OSCAILT_SCRIPT = "viewobjects.php";
addToPageTitle("View Object Usage");

function writeLocalAdminHeader()
{
   global $OSCAILT_SCRIPT;
   ?>
     <TABLE class='admin'>
        <TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?>?sort=none">Raw from Db</a> | <a href="<?=$OSCAILT_SCRIPT?>?sort=type-site">Sort By Type &amp Site Id</a>
        | <a href="<?=$OSCAILT_SCRIPT?>?sort=id-type-site">Sort By Obj Id &amp Type &amp Site Id</a> </TD></TR>
     </TABLE>
   <?
}

function getDropdownList( $ObjectArray, $FilteredSelection)
{
   $js_string = "<option value=\"No Filter\">No Filter</option>";
   $form_string ="<option value=\"";
   foreach ( $ObjectArray as $each_type)
   {
       if ($each_type == $FilteredSelection)
       {
            $js_string .= "<option selected value=\"" . $each_type ."\">" . $each_type . "</option>";
       }
       else
       {
            $js_string .= $form_string . $each_type ."\">" . $each_type . "</option>";
       }
   }

   return $js_string;
}
function getHeadings( $HeadingsList)
{
   $table_str = "";
   foreach ( $HeadingsList as $each_heading)
   {
       $table_str .= "<th class=admin>&nbsp;".ucfirst($each_heading)."&nbsp;</th>";
   }

   return $table_str;
}
// The input is an array of the meta data tags to use
function getDataFields($ObjectPtr, $DataList)
{
   $table_str = "";
   foreach ( $DataList as $each_field)
   {
       if (trim($each_field) == "protected") {
           $each_value = $ObjectPtr->protected ? "<b>yes</b>" : "no";
       } else {
           $each_value = $ObjectPtr->getMeta($each_field);
       }
       $table_str .= "<td class=admin>&nbsp;".$each_value."&nbsp;</td>";
   }

   return $table_str;
}
/******************************************************************************/
/* This function displays a list of all the object ids created and their type */
/* It can sort them by type, site and object id                               */
/* It can also filter by object type, site id and oscailt class. Any or all   */
/* filters can be switched on or off                                          */
/******************************************************************************/
function writeObjectList( $sort_type='none')
{
   global $legalDataTypes, $system_config, $userLanguage, $obj_man, $OSCAILT_SCRIPT;

   // Making 2 arrays. They must be in sync. It can be done better, but too much hassel
   $FilterHdrArray=array("feedimport" => array("page limit","url", "title","convert from UTF-8", "republish"),
                        "feedexport" => array("feedtype","feedversion"),
                        "document" => array("protected"),
                        "box" => array("protected"),
                        "link" => array("link URL", "protected"),
                        "list" => array("Bullet Type"),
                        "site" => array("Use Non-Default Style Sheet Files", "protected"));
   $FilterArray = array("feedimport" => array("pagelimit","url", "title","convertfromutf8", "show_republish"),
                        "feedexport" => array("feedtype","feedversion"),
                        "document" => array("protected"),
                        "box" => array("protected"),
                        "link" => array("linkdestination", "protected"),
                        "list" => array("bullettype"),
                        "site" => array("different_css", "protected"));
   //$available_objects = $obj_man->obj_set->getAllObjectIDs();
   $available_objects = $obj_man->obj_set->getObjectStubs();
   $total_objs = count($available_objects);

   $filter_object_on = false;
   $filter_site_on = false;
   $filter_class_on = false;
   $def_filter_object="No Filter";
   $def_filter_site  ="No Filter";
   $def_filter_class ="No Filter";
   if ( isset($_REQUEST['filter_object'])) { $def_filter_object = $_REQUEST['filter_object'];}
   if ( isset($_REQUEST['filter_site'])) { $def_filter_site = $_REQUEST['filter_site'];}
   if ( isset($_REQUEST['filter_class'])) { $def_filter_class = $_REQUEST['filter_class'];}

   # The value No Filter can still be set in which case it is really off.
   if ( $def_filter_object != "No Filter") { $filter_object_on = true; }
   if ( $def_filter_site != "No Filter") { $filter_site_on = true; }
   if ( $def_filter_class != "No Filter") { $filter_class_on = true; }

   # If we filter on the object name then we will display the object name.
   $col_span_size = 8;
   if ( $filter_object_on == true ) 
   {
       $show_pagelayout = false;
       $show_newslink = false;
       // Not all object types have a layout reference, so we only get it for those that do.
       $pageRefTypes = array("document", "preferences", "box", "list", "newswire", "feature", "article", "contact", "publish", "events", "search", "gallery", "archive", "comments", "feedimport");

       if (in_array($def_filter_object, $pageRefTypes)) $show_pagelayout = true;

       if ($def_filter_object == "picturebox" || $def_filter_object == "headlinebox" ) $show_newslink = true;

       # **** BIG FIX TO BE DONE **** Assuming language code is en. For other languages
       # we need to move this code to later when we know the code. In fact if you have 2
       # or more per object type then you need 2 or more calls.
       # You could make use of IndyObjectSet getObjectAvailableLanguages()
       $obj_name_info = $obj_man->obj_set->getObjectInfoByTypename($def_filter_object, "en" );
       $page_obj_names= $obj_man->obj_set->getObjectInfoByTypename("page", "en" );

       if ($show_newslink == true)
           $news_obj_names= $obj_man->obj_set->getObjectInfoByTypename("newswire", "en" );

       $col_span_size = 9;
       if(in_array($def_filter_object, array_keys($FilterArray)) == true) {
          $col_span_size = $col_span_size + count(array_values($FilterArray[$def_filter_object]));
       }
   }
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=<?=$col_span_size?>>Objects: Total = <?=$total_objs?> </td>
   </tr>
   <?
   if ($sort_type != 'none')
   {
       if ($sort_type == 'type-site') $sort_text = "Type and Site";
       else if ($sort_type == 'id-type-site') $sort_text = "Type, Site and Object Id";
       ?>
       <tr class=admin>
         <th class=admin colspan=<?=$col_span_size?>>Sorted by <?=$sort_text?> </td>
       </tr>
       <?
   }
   if ( $filter_object_on == true ) 
   {
       $t_object_url = "<a href='admin.php?action=list&site_id=1&obj_type=".$def_filter_object."&obj_language=en'>".  $def_filter_object."</a>";
       // Previously was just $def_filter_object
       ?>
       <tr class=admin>
         <td class=admin colspan=<?=$col_span_size?>>Filtering by Object Type: <b><?=$t_object_url?></b> </td>
       </tr>
       <?
   }

   # Generate the form selections ... would it be more efficient to print each selection or
   # as here generate fulls string and then print? Probably sub-millisecond difference anyhow.
   ?>
   <FORM name="viewobjsform" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <tr class=admin>
      <td align=center colspan=<?=$col_span_size?>><input type=submit name=filter_tag value="Filter"> by Object Type
      <select name='filter_object' > 
      <?

      $js_string = getDropdownList(array_keys($legalDataTypes), $def_filter_object);
      echo $js_string;

      ?>
      </select> 
      Site <select name='filter_site' > 
      <?

      $js_string = getDropdownList($obj_man->obj_set->sites, $def_filter_site);
      echo $js_string;

      ?>
      </select> 
      Oscailt Class <select name='filter_class' > 
      <?
      $OscailtClasses = array_unique(array_values($legalDataTypes));
      $js_string = getDropdownList($OscailtClasses, $def_filter_class);
      echo $js_string;
      ?>
      </select> 
      </td>
   </tr>
   </form>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;Object ID&nbsp;</th>
   <?
   if ( $filter_object_on == true ) {
      ?>
      <th class=admin>&nbsp;Site&nbsp;</th>
      <th class=admin>Language Code</th>
      <th class=admin>&nbsp;Object Name&nbsp;</th>
      <?
   } else {
      ?>
      <th class=admin>&nbsp;Object Type&nbsp;</th>
      <th class=admin>&nbsp;Site&nbsp;</th>
      <th class=admin>Language Code</th>
      <?
   }
   ?>
      <th class=admin>&nbsp;Oscailt Class&nbsp;</th>
      <th class=admin>&nbsp;Object Cache File&nbsp;</th>
      <th class=admin>&nbsp;File Exists</th>
   <?
   // Add a contains field
   if ( $filter_object_on == true && ($show_pagelayout == true || $show_newslink == true)) {
      // Not all object types have a layout reference, so we only get it for those that do.
      if($show_pagelayout == true) {
          ?> <th class=admin>&nbsp;Page Layout Obj&nbsp;</th> <?

      } else if($show_newslink == true) {
          ?> <th class=admin>&nbsp;Newslink Obj&nbsp;</th> <?

      } else {
          ?> <th class=admin>&nbsp;Page Layout Obj&nbsp;</th> <?
      }

      if(in_array($def_filter_object, array_keys($FilterArray)) == true) {
          $t_headings = getHeadings(array_values($FilterHdrArray[$def_filter_object]));
          echo $t_headings;
      }
   } else {
      if(in_array($def_filter_object, array_keys($FilterArray)) == true) {
          $t_headings = getHeadings(array_values($FilterHdrArray[$def_filter_object]));
          echo $t_headings;
      }
   }
   ?>
   </tr>
   <?

   // Grab the data and store in arrays as needed per sort. The PHP sort will rejig all
   // the data in all the arrays passed to it.
   $counter = 0;
   foreach($available_objects as $obj_stub)
   {
      $arr_order[] = $counter++;
      $arr_obj_stub[] = $obj_stub;
      $arr_obj_site[] = $obj_stub->site_id;
      $arr_obj_type[] = $obj_stub->obj_type;
      if ($sort_type == 'id-type-site')
      {
          $arr_obj_id[] = $obj_stub->obj_id;
      }
   }

   # Best reading up the PHP array_multisort to figure this out..
   if ($sort_type == 'type-site')
   {
      array_multisort($arr_obj_site, $arr_obj_type, $arr_order);
   }
   else if ($sort_type == 'id-type-site')
   {
      array_multisort($arr_obj_site, $arr_obj_type, $arr_obj_id, $arr_order);
   }

   $iRow = 0;
   for($aIndex=0;$aIndex < $counter ;$aIndex++)
   {
      $sortIndex = $arr_order[$aIndex];
      if ($sort_type == 'id-type-site')
      {
          $ob_id = $arr_obj_id[$aIndex];
      }
      else
      {
          $ob_id = $arr_obj_stub[$sortIndex]->obj_id;
      }
      $ob_lang = $arr_obj_stub[$sortIndex]->primary_language_code;

      $lang_list = $obj_man->obj_set->getObjectAvailableLanguages($ob_id);

      $obj_typename = strtolower($arr_obj_type[$aIndex]);
      $ob_site = $arr_obj_site[$aIndex];
      $ob_class= $legalDataTypes[$obj_typename];

      # This filtering could be done before the sorting. It would be a lot more efficient
      # but it was done here, because it allowed for easier debug during development.
      if ( $filter_object_on == true && $obj_typename != $def_filter_object) { continue; }
      if ( $filter_site_on == true && $ob_site != $def_filter_site) { continue; }
      if ( $filter_class_on == true && $ob_class != $def_filter_class) { continue; }

      $iRow++;

      if ( $filter_object_on == true )
      {
          $ob_name = $obj_name_info[$ob_id];
      }


      if($obj_typename == "") $obj_typename = "<span class='error'>unknown object type</span>";
      $ob_cache_file = $obj_man->obj_set->getIncludeFileRef($ob_id);

      $ob_file_exists = "<b>No</b>";
      if (file_exists($ob_cache_file)) { $ob_file_exists = "Yes"; }

      // If there is more than 1 language then get the list of associated files.
      if (count($lang_list) > 1) { 
         $lang_files = "";
         $lang_file_exists = "";
         foreach($lang_list as $lang) {
             $lang_cache_file = $obj_man->obj_set->getIncludeFileRefByLang($ob_id, $lang);

             if (file_exists($lang_cache_file)) 
	         $lang_file_exists .= "<BR>Yes";
             else
	         $lang_file_exists .= "<BR>No";

             $lang_files .= "<BR>" . $lang_cache_file;
         }
         $ob_cache_file .= "<BR>".$lang_files;
         $ob_file_exists .="<BR>".$lang_file_exists;
      }

      //if (($filter_object_on != true) && (count($lang_list) > 1)) { 
      if (count($lang_list) > 1) { 
         $ob_lang = "";
         foreach($lang_list as $lang) {
             $ob_lang .= $lang." ";
         }
      }
      ?>
      <tr class=admin>
         <td class=admin align=center>&nbsp;<?=($iRow)?>&nbsp;</td>
         <td class=admin align=center>&nbsp;<?=$ob_id?>&nbsp;</td>
      <?
      // Check for isProtected and display coloured ball -red or green
      if ($filter_object_on != true) { 
         if (count($lang_list) > 1) { 
            $t_object_grp_url = "";
            $t_object_grp_base = "<a href='admin.php?action=list&site_id=".$ob_site."&obj_type=".$obj_typename."&obj_language=";
	    $t_cnt =0;
            foreach($lang_list as $lang) {
                $t_object_grp_url .= $t_object_grp_base . $lang ."'>".ucfirst($obj_typename)."_".$lang."</a>";
	        $t_cnt++;
		if ($t_cnt < count($lang_list) ) $t_object_grp_url .= "<BR>&nbsp;";
            }
            ?> <td class=admin align=left  >&nbsp;<?=$t_object_grp_url?>&nbsp;</td> <?
         } else {
            $t_object_grp_url = "<a href='admin.php?action=list&site_id=".$ob_site."&obj_type=".$obj_typename."&obj_language=en'>".ucfirst($obj_typename)."</a>";
            ?> <td class=admin align=left  >&nbsp;<?=$t_object_grp_url?>&nbsp;</td> <?
         }
      }
      ?>
         <td class=admin align=center>&nbsp;<?=$ob_site?>&nbsp;</td>
         <td class=admin align=center>&nbsp;<?=$ob_lang?>&nbsp;</td>
      <?
      if ($filter_object_on == true) { 
         ?> <td class=admin align=center>&nbsp;<?=$ob_name?>&nbsp;</td> <?
      }
      ?>
         <td class=admin align=center>&nbsp;<?=$ob_class?>&nbsp;</td>
         <td class=admin align=left><?=$ob_cache_file?></td>
         <td class=admin align=center><?=$ob_file_exists?></td>
      <?

      if ($filter_object_on == true) { 
         $new_obj = $obj_man->obj_set->fetchObject($ob_id, "en");
	 if ($new_obj == false ) {
            $page_layout_id = " N/A";
         } else {
            // Not all object types have a layout reference, so we only get it for those that do.
            // Like headlines has newswireobject.
            $page_layout_id = false;
            if ($show_pagelayout == true) {
                $page_layout_id = $new_obj->getPageLayoutRef();
                if ($page_layout_id == false ) 
	           $page_layout_id = "Not Set Yet";
	        else 
                {
	           if (isset($page_obj_names[$page_layout_id]))
	               $page_layout_id .= " " .$page_obj_names[$page_layout_id];
                   else
	               $page_layout_id = " Set to None ";
                }
            }
	    else if($show_newslink == true) {
                if ($new_obj->getMeta('newswireobject') > 0)
                    $page_layout_id = $new_obj->getMeta('newswireobject');

	        if ($page_layout_id == false ) 
	            $page_layout_id = "Not Set Yet";
	        else 
                {
	           if (isset($news_obj_names[$page_layout_id]))
	               $page_layout_id .= " " .$news_obj_names[$page_layout_id];
                   else
	               $page_layout_id = " Set to None ";
                }
            }
         }
         if ($show_pagelayout == true || $show_newslink == true) {
            ?>
            <td class=admin align=center>&nbsp;<?=$page_layout_id?>&nbsp;</td>
            <?
         }
         if(in_array($def_filter_object, array_keys($FilterArray)) == true && $new_obj != false ) {
              $t_data = getDataFields($new_obj, array_values($FilterArray[$def_filter_object]));
              echo $t_data;
         }
      }
      ?>
      </tr>
      <?
   }
   ?>
   </table>
   <?

   // writeJSFunctions();
}

function writeJSFunctions()
{
    global $legalDataTypes, $system_config, $obj_man;

    # This function is no longer used. It could be updated to re-submit on selection above.
    ?>
    <script type="text/javascript" language="Javascript">
         function checkFilterItemChange()
         {
            if(document.viewobjsform.filter_object.value=="tag_1")
            {
               // Do the submit
            }
         }
    </script>
    <?
}

function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function loadManagedObjectSet()
{
   global $system_config, $OSCAILT_SCRIPT, $obj_man;
   # This ultimately loads the object stubs (i.e. basic info) for all known objects.
   $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $OSCAILT_SCRIPT);
   $obj_man->obj_set = new indyObjectSet($obj_man->type_dir, $obj_man->storage);
   $sites = array("*");
   $types = array("*");
   if(!$obj_man->obj_set->load($sites, $types, $obj_man->action_req))
   {
      $obj_man->writeUserMessageBox();
      writeError("Programme Error: Failed to Load Set of Managed Objects.");
      return false;
   }
   return true;
}

ob_start();
$obj_man = null;
$admin_table_width = "100%";
// loadManagedObjectSet();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("editredirects.php","Edit Friendly URLs");

   if($editor_session->editor->allowedReadAccessTo("editdataobjects"))
   {
      writeLocalAdminHeader();
      if( loadManagedObjectSet() == true ) {
         $sort_mode = 'none';
         if ( isset($_REQUEST['sort']))
         {
             $sort_mode = $_REQUEST['sort'];
         }
         writeObjectList($sort_mode);
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>