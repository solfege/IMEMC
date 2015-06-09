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
$textLabels = array("title" => "View Object Usage",
	            "view_objects" => "View Objects",
	            "view_templates" => "View Templates",
	            "no_filter_text" => "No Filter",
	            "object_totals" => "Objects: Total",
	            "object_id_text" => "Object Id",
	            "object_type_text" => "Object Type",
	            "sorted_by_text" => "Sorted by",
	            "by_object_type_text" => "Filter by Object Type",
	            "site_word" => "Site",
	            "language_code" => "Language Code",
	            "object_name_text" => "Object Name",
	            "oscailt_class" => "Oscailt Class",
	            "object_cache_file" => "Object Cache File",
	            "file_exists" => "File Exists",
	            "page_layout" => "Page Layout Obj",
	            "newslink_obj_text" => "Newslink Obj",
	            "FilterBtn" => "Filter",
	            "CreateNewBtn" => "Create New Role",
	            "filtering_by" => "Filtering by Object Type",
	            "raw_mode" => "Raw from Db",
	            "sort_type_site" => "Sort by Type &amp; Site Id",
	            "sort_objtypesite" => "Sort by Obj Id &amp; Type &amp; Site Id",
	            "template_name" => "Template Name",
	            "storage_file" => "Storage File",
	            "exist_word" => "Exists",
	            "template_name" => "Template Name");


$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "viewobjects") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the View Objects. -Using defaults",""));
} else {
    foreach (array_keys($textLabels) as $str_key ) {
        if (trim($textObj->getString($str_key)) != "" )
            $textLabels[$str_key] = $textObj->getString($str_key);
	else
        {
            if ($system_config->user_error_reporting >= 8) $textLabels[$str_key] .= " using default ";
	}
    }
}

addToPageTitle($textLabels["title"]);

function writeLocalAdminHeader()
{
   global $OSCAILT_SCRIPT, $textLabels;
   ?>
     <TABLE class='admin'>
     <TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?>"> <?=$textLabels['view_objects']?></a> | <a href="<?=$OSCAILT_SCRIPT?>?view=templates"> <?=$textLabels['view_templates']?></a> </TD></TR>
     </TABLE>
   <?
   /*
        <TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?>?sort=none">Raw from Db</a> | <a href="<?=$OSCAILT_SCRIPT?>?sort=type-site">Sort By Type &amp Site Id</a>
   */
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
   //$table_str = "";
   $table_list = array();
   foreach ( $DataList as $each_field)
   {
       if (trim($each_field) == "protected") {
           $each_value = $ObjectPtr->protected ? "<b>yes</b>" : "no";
       } else if (substr($each_field,0,4) == "date" ) {
           $each_value = $ObjectPtr->getMeta($each_field);
	   if (strtolower($each_value) == "on") {
               $each_value .= "<BR>";
               $each_value .= $ObjectPtr->getMeta("activate_start_day") ."-".($ObjectPtr->getMeta("activate_start_month")+1);
               $each_value .= "-&gt;";
               $each_value .= $ObjectPtr->getMeta("activate_end_day") ."-".($ObjectPtr->getMeta("activate_end_month")+1);
	   }
       } else if (strpos($each_field,"contained") !== false ) {
	       $each_value ="";
	       foreach ($ObjectPtr->containedItems[$each_field] as $contain_item)
	       {
                  if (is_object($contain_item)) {	
	             if ($each_value != "") $each_value .= "<BR>";
	             $each_value .= $contain_item->id . " ";
	             global $obj_man;
                     $new_obj = $obj_man->obj_set->fetchObject($contain_item->id, "en");
		     if ($new_obj != null) {
                        if ($each_field != "contained") {
                            $t_object_url = "<a href='?filter_object=".strtolower($new_obj->type)."'>". ucfirst(strtolower($new_obj->type))."</a>";
	                    // $each_value .= "(".ucfirst(strtolower($new_obj->type)).") ";
	                    $each_value .= "(".$t_object_url.") ";
	                    $each_value .= ucfirst(strtolower($new_obj->name()));
			} else {
	                    $each_value .= ucfirst(strtolower($new_obj->type));
		        }
		     }
	          }
	       }
       } else {
           $each_value = $ObjectPtr->getMeta($each_field);
       }
       $table_list[$each_field] = $each_value;
       //$table_str .= "<td class=admin>".$each_value."&nbsp;</td>";
   }

   return $table_list;
}
/******************************************************************************/
/* This function displays a list of all the object ids created and their type */
/* It can sort them by type, site and object id                               */
/* It can also filter by object type, site id and oscailt class. Any or all   */
/* filters can be switched on or off                                          */
/******************************************************************************/
function writeObjectList( $sort_type='none')
{
   global $legalDataTypes, $system_config, $userLanguage, $obj_man, $OSCAILT_SCRIPT, $pageLanguage;
   global $textLabels;

   // Making 2 arrays. They must be in sync. It can be done better, but too much hassel
   $FilterHdrArray=array("feedimport" => array("page limit","url", "title","convert from UTF-8", "republish"),
                        "feedexport" => array("feedtype","feedversion"),
                        "document" => array("protected"),
                        "box" => array("date activated", "contains" ),
                        "bar" => array("contains links to" ),
                        "menu" => array("contains links to" ),
                        "link" => array("link URL", "protected"),
                        //"list" => array("contains links to", "Bullet type"),
                        "page" => array("Header", "Left Col", "Insets ", "Right Col", "Footer"),
                        "list" => array("contains links to"),
                        "gallery" => array("Display Type"),
                        "picturebox" => array("Using Featured Image","Image File"),
                        "site" => array("Use Non-Default Style Sheet Files", "protected"));
   $FilterArray = array("feedimport" => array("pagelimit","url", "title","convertfromutf8", "show_republish"),
                        "feedexport" => array("feedtype","feedversion"),
                        "document" => array("protected"),
                        "box" => array("date_activated", "contained" ),
                        "bar" => array("contained" ),
                        "menu" => array("contained" ),
                        "link" => array("linkdestination", "protected"),
                        //"list" => array("contained", "bullettype"),
                        "page" => array("headercontained", "leftcolumncontained", "mainpagecontained", "rightcolumncontained", "footercontained"),
                        "list" => array("contained" ),
                        "gallery" => array("show_type" ),
                        "picturebox" => array("usefeaturedphoto", "image" ),
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
       $pageRefTypes = array("document", "preferences", "box", "list", "newswire", "feature", "article", "contact", "publish", "events", "search", "gallery", "archive", "comments", "feedimport", "citylist");

       if (in_array($def_filter_object, $pageRefTypes)) $show_pagelayout = true;

       if ($def_filter_object == "picturebox" || $def_filter_object == "headlinebox" ) $show_newslink = true;

       # **** BIG FIX TO BE DONE **** Assuming language code is en. For other languages
       # we need to move this code to later when we know the code. In fact if you have 2
       # or more per object type then you need 2 or more calls.
       # You could make use of IndyObjectSet getObjectAvailableLanguages()
       $t_lang_code = "pl";
       // Below doesnt result in correct langs loaded. Needs more thought.
       // if (isset($pageLanguage) ) $t_lang_code = $pageLanguage->getLanguageCodePrefix();
       // else if (isset($userLanguage) ) $t_lang_code = $userLanguage->getLanguageCodePrefix();

       $obj_name_info = $obj_man->obj_set->getObjectInfoByTypename($def_filter_object, $t_lang_code );
       $page_obj_names= $obj_man->obj_set->getObjectInfoByTypename("page", $t_lang_code );

       if ($show_newslink == true)
           $news_obj_names= $obj_man->obj_set->getObjectInfoByTypename("newswire", $t_lang_code );

       $col_span_size = 9;
       if(in_array($def_filter_object, array_keys($FilterArray)) == true) {
          $col_span_size = $col_span_size + count(array_values($FilterArray[$def_filter_object]));
       }
   }
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=<?=$col_span_size?>> <?=$textLabels['object_totals']?> = <?=$total_objs?> </th>
   </tr>
   <?
   if (isset($_REQUEST['sort_object'])) {
       if ($_REQUEST['sort_object'] == "Db Rec-Id") $sort_type = "none";
       else if ($_REQUEST['sort_object'] == "Type+Site-Id") $sort_type = "type-site";
       else if ($_REQUEST['sort_object'] == "Obj-Id + Type + Site-Id") $sort_type = "id-type-site";
   }
   if ($sort_type != 'none')
   {
       if ($sort_type == 'type-site') $sort_text = "Type and Site";
       else if ($sort_type == 'id-type-site') $sort_text = "Type, Site and Object Id";
       ?>
       <tr class=admin>
         <th class=admin colspan=<?=$col_span_size?>> <?=$textLabels['sorted_by_text']?> <?=$sort_text?> </td>
       </tr>
       <?
   }
   if ( $filter_object_on == true ) 
   {
       $t_object_url = "<a href='admin.php?action=list&site_id=1&obj_type=".$def_filter_object."&obj_language=en'>".  $def_filter_object."</a>";
       // Previously was just $def_filter_object
       ?>
       <tr class=admin>
         <td class=admin colspan=<?=$col_span_size?>> <?=$textLabels['filtering_by']?>: <b><?=$t_object_url?></b> </td>
       </tr>
       <?
   }

   # Generate the form selections ... would it be more efficient to print each selection or
   # as here generate fulls string and then print? Probably sub-millisecond difference anyhow.
   ?>
   <FORM name="viewobjsform" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <tr class=admin>
      <td align=center colspan=<?=$col_span_size?>>
      Sort by <select name='sort_object' onchange='submit();'> 
      <?

      $default_select = "Db Rec-Id";
      if (isset($_REQUEST['sort_object'])) $default_select = $_REQUEST['sort_object'];
      $js_string = getDropdownList(array("Db Rec-Id","Type+Site-Id","Obj-Id + Type + Site-Id"), $default_select, true);
      echo $js_string;

      ?>
      </select> &nbsp;
      <?=$textLabels['by_object_type_text']?>
      <select name='filter_object' onchange='submit();'> 
      <?

      
      $objectTypes = array_keys($legalDataTypes);
      sort($objectTypes);
      $js_string = getDropdownList($objectTypes, $def_filter_object);
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
      <th class=admin>&nbsp;<?=$textLabels['object_id_text']?>&nbsp;</th>
   <?
   if ( $filter_object_on == true ) {
      ?>
      <th class=admin>&nbsp;<?=$textLabels['site_word']?>&nbsp;</th>
      <th class=admin><?=$textLabels['language_code']?></th>
      <th class=admin>&nbsp;<?=$textLabels['object_name_text']?>&nbsp;</th>
      <?
   } else {
      ?>
      <th class=admin>&nbsp;<?=$textLabels['object_type_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['site_word']?>&nbsp;</th>
      <th class=admin><?=$textLabels['language_code']?></th>
      <th class=admin>&nbsp;<?=$textLabels['oscailt_class']?>&nbsp;</th>
      <?
   }
   ?>
      <th class=admin>&nbsp;<?=$textLabels['object_cache_file']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['file_exists']?></th>
   <?
   // Add a contains field
   if ( $filter_object_on == true && ($show_pagelayout == true || $show_newslink == true)) {
      // Not all object types have a layout reference, so we only get it for those that do.
      if($show_pagelayout == true) {
          ?> <th class=admin>&nbsp;<?=$textLabels['page_layout']?>&nbsp;</th> <?

      } else if($show_newslink == true) {
          ?> <th class=admin>&nbsp;<?=$textLabels['newslink_obj_text']?>&nbsp;</th> <?

      } else {
          ?> <th class=admin>&nbsp;<?=$textLabels['page_layout']?>&nbsp;</th> <?
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
         ?> <td class=admin align=center><?=$ob_name?></td> <?
      } else {
	 ?> <td class=admin align=center>&nbsp;<?=$ob_class?>&nbsp;</td> <?
      }
      ?>
         <td class=admin align=left><?=$ob_cache_file?></td>
         <td class=admin align=center><?=$ob_file_exists?></td>
      <?

      if ($filter_object_on == true) { 
	$t_data = "";
	$t_array = array();
	$page_layout_txt = "";
	foreach($lang_list as $lang) {

	    $new_obj = $obj_man->obj_set->fetchObject($ob_id, $lang);
	    if ($new_obj == false ) {
		$page_layout_txt .= " N/A";
	    } else {
		// Not all object types have a layout reference, so we only get it for those that do.
		// Like headlines has newswireobject.
		$page_layout_id = false;
		if ($show_pagelayout == true) {
			$page_layout_id = $new_obj->getPageLayoutRef();
			if ($page_layout_id == false ) 
			    $page_layout_txt .= "Not Set Yet ";
			else 
			{
			    if (isset($page_obj_names[$page_layout_id]))
				$page_layout_txt .= " " .$page_obj_names[$page_layout_id];
			    else
				$page_layout_txt .= "Set to None ";
			}
		}
		else if($show_newslink == true) {
			if ($new_obj->getMeta('newswireobject') > 0)
			    $page_layout_id = $new_obj->getMeta('newswireobject');

			if ($page_layout_id == false ) 
			    $page_layout_txt .= "Not Set Yet ";
			else
			{
			    if (isset($news_obj_names[$page_layout_id]))
				$page_layout_txt .= " " .$news_obj_names[$page_layout_id];
			    else
				$page_layout_txt .= "Set to None ";
			}
		}
	   }
	   $page_layout_txt .= "<BR>";
	   if(in_array($def_filter_object, array_keys($FilterArray)) == true && $new_obj != false ) {
	        $t_array[$lang] = getDataFields($new_obj, array_values($FilterArray[$def_filter_object]));
	   }
	}
	?>
	<td class=admin align=center><?=$page_layout_txt?></td>
	<?
	if(count($t_array) > 0 ) {
	    $tx_each_element = "";
	    foreach(array_values($FilterArray[$def_filter_object]) as $table_cell) {
		$tx_each_element = "";
		foreach($lang_list as $lang) {
			foreach ($t_array[$lang] as $t_key => $sub_array) {
				if ($t_key != $table_cell) continue;
				// echo "Key=".$t_key." (NotArray)[".$sub_array."]&nbsp;<br>"; 
				$tx_each_element .= $sub_array."<BR>";
			}
		}
		echo "<td class=admin> ".$tx_each_element."&nbsp;</td>"; 
	    }
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
/******************************************************************************/
/* This function displays a list of templates. If the template directory does */
/* not exist then it tries to fix it.                                         */
/******************************************************************************/
function writeTemplateList( )
{
   global $system_config, $userLanguage, $obj_man, $OSCAILT_SCRIPT;
   global $languageList;
   global $textLabels;

   $templates_loaded = $obj_man->loadTemplateSet();
   $create_xml_file = true;

   if ($templates_loaded == false) {
       $obj_man->writeUserMessageBox();

       if (file_exists($system_config->object_template_store."site_section_templates"))
       {
           // Check template.xml is present.
           if(file_exists($system_config->object_template_store."/templates.xml"))
           {
               echo "<div class='user-message'>Moving templates.xml file to ".$system_config->object_template_store." directory </div>";
               rename($system_config->object_template_store."/templates.xml", $system_config->object_template_store."site_section_templates/templates.xml");
               echo "<div class='user-message'>Templates setup now ready</div>";
           } else {
               echo "<div class='user-message'>Cannot find template.xml file in directory ".$system_config->object_template_store."<BR>Creating empty templates.xml file.</div>";
	       if ($create_xml_file == true) {
                   $xml_file = $system_config->object_template_store."site_section_templates/templates.xml";
                   $fp = fopen($xml_file,"w");
		   if ($fp != null) {
                       if (flock($fp, LOCK_EX))
                       {
			   // For some reason the \n has to be in quotes otherwise it doesnt work.
                           fputs($fp, '<?xml version="1.0" encoding="ISO-8859-1" ?>'."\n");
                           fputs($fp, '<indyObjectSet>'."\n");
                           fputs($fp, '</indyObjectSet>'."\n");
                       }
                       flock($fp, LOCK_UN);
                       fclose($fp);
		   } else {
                       echo "<div class='user-message'>No permission to write ".$xml_file." file.</div>";
		   }
               }
           }
       } else {
         echo "<div class='user-message'>Creating templates directory and moving templates.xml file to it </div>";
         if(!mkdir($system_config->object_template_store."site_section_templates/", $system_config->default_writable_directory_permissions))
         {

             echo "<div class='user-message'>Cannot create directory ".$system_config->object_template_store."site_section_templates/</div>";
         }
       }

       return;
   }

   // op_r($obj_man->template_set);
   $shortenPath = true;
   $showlinks   = true;
   $shortenLen = 0;
   $locate_label = "Located";
   if ($shortenPath == true) {
      $shortenLen = strlen(dirname($obj_man->template_set->index_file_map['templates']))+1;
      $locate_label = "Template files";
   }

   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=8>Templates Directory: <?=dirname($obj_man->template_set->index_file_map['templates'])?> <br> <small>Template file must exist if it is to be used. See 'Save Template' option in module layout screens</small></th>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
   <th class=admin>&nbsp;<?=$textLabels['template_name']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['object_type_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['site_word']?>&nbsp;</th>
      <th class=admin><?=$textLabels['language_code']?></th>
      <th class=admin>&nbsp;<?=$textLabels['storage_file']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$locate_label?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['exist_word']?>&nbsp;</th>
   <?

   $sort_object = true;
   // if ( isset($_REQUEST['sort_object'])) { $sort_object = $_REQUEST['sort_object'];}

   // Grab the data and store in arrays as needed per sort. The PHP sort will rejig all
   // the data in all the arrays passed to it.
   $shortenPath = true;

   // Grab the data and store in arrays as needed per sort. The PHP sort will rejig all
   // the data in all the arrays passed to it.
   $counter = 0;
   $itemCollection = $obj_man->template_set->itemCollection;
   foreach ($itemCollection as $templateObj) {
	   if (is_object($templateObj)) {
               $arr_order[] = $counter++;
               $arr_obj_stub[] = $templateObj;
               $arr_obj_type[] = $templateObj->obj_type;
               $arr_obj_name[] = $templateObj->obj_id;
	   }
   }
   if ($sort_object == 'true')
   {
      array_multisort($arr_obj_type, $arr_obj_name, $arr_order);
   }
   if ($counter == 0) {
       ?>
       <tr class=admin>
       <td class=admin colspan=8>There are no templates created yet. Use 'Save Template' option in module layout screens to begin saving templates. </td>
       </tr>
       <?
   }

   for($aIndex=0;$aIndex < $counter ;$aIndex++)
   {
      $sortIndex = $arr_order[$aIndex];
      if ($sort_object == 'true')
      {
          $templateObj = $arr_obj_stub[$sortIndex];
      } else {
          $templateObj = $arr_obj_stub[$aIndex];
      }
      if (is_array($templateObj)) {
          foreach ($templateObj as $each_obj_key => $each_obj_data) {
	      echo $each_obj_key . "<BR>"; 
	  }
      } else if (is_object($templateObj)) {
	  echo "<tr class=admin>"; 
	  echo "<td class=admin>" . ($aIndex+1). "</td>"; 
	  echo "<td class=admin>" . $templateObj->obj_id . "</td>"; 

          if ($showlinks == true) {
              $t_type = strtolower($templateObj->obj_type);
	      // For templates there is no real site, so default it to 1.
              $t_object_grp_url = "<a href='admin.php?action=list&site_id=1&obj_type=".$t_type."&obj_language=en'>".ucfirst($t_type)."</a>";
	      echo "<td class=admin>" . $t_object_grp_url . "</td>"; 
	  } else {
	      echo "<td class=admin>" . ucfirst(strtolower($templateObj->obj_type)). "</td>"; 
	  }

	  echo "<td class=admin>" . $templateObj->site_id . "</td>"; 
	  echo "<td class=admin align=center>" . $templateObj->primary_language_code . "</td>"; 
	  echo "<td class=admin>" . $templateObj->storage . "</td>"; 
	  $t_file_end = "_".$templateObj->primary_language_code.".xml";
	  if ($shortenPath == true) {
	      echo "<td class=admin>" . substr($templateObj->real_storage,$shortenLen).$t_file_end . "</td>"; 
	  } else {
	      echo "<td class=admin>" . $templateObj->real_storage . "</td>"; 
	  }
	  $t_file = $templateObj->real_storage. $t_file_end;

	  if (file_exists($t_file)) $t_stat = "<b>Yes</b>"; 
	  else $t_stat = " No "; 
	  $languages = $languageList->getLanguages();
	  $lingo = "";
          foreach($languages as $lang)
          {
              $lprefix = $lang->getLanguageCodePrefix();
              $lfname = $templateObj->real_storage.'_'.$lprefix.'.xml';
              if(file_exists($lfname) )
              {
	           $lingo .= "<br>" . $lfname ;
                   // $avlangs[] = $lprefix; 
              }            
          }
    
	  echo "<td class=admin>" . $t_stat . $lingo . "</td></tr>"; 
      } else {
          echo "<TR><TD colspan=8>template_set is not an array </TD>";
      }
   }

   ?>
   </table><br>
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
   writeAdminHeader("viewobjects.php?view=templates","View Templates",array("editredirects.php" => "Edit Friendly URLs"));
   

   if($editor_session->editor->allowedReadAccessTo("editdataobjects"))
   {
      // writeLocalAdminHeader();
      $sort_mode = 'none';
      if ( isset($_REQUEST['sort'])) $sort_mode = $_REQUEST['sort'];

      if ( isset($_REQUEST['view']) && $_REQUEST['view'] == "templates")
      {
          if( loadManagedObjectSet() == true ) {
              writeTemplateList();
          }
      } else {
          if( loadManagedObjectSet() == true ) {
              writeObjectList($sort_mode);
          }
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>

