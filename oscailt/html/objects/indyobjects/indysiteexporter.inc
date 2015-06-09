<?
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
require_once "objects/indyobjects/indyimportexport.inc";

class indySiteExporter extends indyImportExport
{

   function indySiteExporter(&$obj_set)
   {
      global $system_config;
      $this->indyImportExport($system_config->object_export_dir, $obj_set);
      $this->metadata = new indyItemSet();
   }

   function readChoices()
   {
      global $languageList, $topicList, $typeList, $regionList, $roleList;
      global $userLanguage, $pageLanguage;

      if(isset($_REQUEST['export_id']))
      {
         $this->metadata->name = $_REQUEST['export_id'];
      }
      if(isset($_REQUEST['export_title']))
      {
         $this->metadata->setMeta("title", $_REQUEST['export_title']);
      }
      if(isset($_REQUEST['export_description']))
      {
         $this->metadata->setMeta("description", $_REQUEST['export_description']);
      }
      if(isset($_REQUEST['export_icon_small']))
      {
         $this->metadata->setMeta("iconsmall", $_REQUEST['export_icon_small']);
      }
      if(isset($_REQUEST['export_icon_medium']))
      {
         $this->metadata->setMeta("iconmedium", $_REQUEST['export_icon_medium']);
      }
      if(isset($_REQUEST['export_icon_large']))
      {
         $this->metadata->setMeta("iconlarge", $_REQUEST['export_icon_large']);
      }

      $langs = $languageList->getfLanguages();
      foreach($langs as $l)
      {
         if(isset($_REQUEST['export_language_'.$l->language_id]) && $_REQUEST['export_language_'.$l->language_id] == 'on')
         {
            $this->choices['language'][] = $l->language_id;
         }
      }

      // Display the list of topics, regions and types in the displayed language.
      if (isset($userLanguage)) { $t_lang_id = $userLanguage->language_id; }
      else if (isset($pageLanguage)) { $t_lang_id = $pageLanguage->language_id; }
      else { $t_lang_id = $languageList->getMinLanguageId(); }

      $topics = $topicList->getfTopics($t_lang_id);
      foreach($topics as $t)
      {
         if(isset($_REQUEST['export_topic_'.$t->topic_id]) && $_REQUEST['export_topic_'.$t->topic_id] == 'on')
         {
            $this->choices['topic'][] = $t->topic_id;
         }
      }
      $types = $typeList->getTypes($t_lang_id);
      foreach($types as $t)
      {
         if(isset($_REQUEST['export_type_'.$t->type_id]) && $_REQUEST['export_type_'.$t->type_id] == 'on')
         {
            $this->choices['type'][] = $t->type_id;
         }
      }
      $regions = $regionList->getRegions($t_lang_id);
      foreach($regions as $r)
      {
         if(isset($_REQUEST['export_region_'.$r->region_id]) && $_REQUEST['export_region_'.$r->region_id] == 'on')
         {
            $this->choices['region'][] = $r->region_id;
         }
      }
      $rs = $roleList->getRoles($t_lang_id);
      foreach($rs as $r)
      {
         if(isset($_REQUEST['export_role_'.$r->role_id]) && $_REQUEST['export_role_'.$r->role_id] == 'on')
         {
            $this->choices['role'][] = $r->role_id;
         }

      }
      $this->choices['css'] = (isset($_REQUEST['export_css']) && $_REQUEST['export_css'] == 'on') ? true : false;
      $this->choices['redirects'] = (isset($_REQUEST['export_redirects']) && $_REQUEST['export_redirects'] == 'on') ? true : false;
      $this->choices['files'] = (isset($_REQUEST['export_files']) && $_REQUEST['export_files'] == 'on') ? true : false;
      $this->choices['links'] = (isset($_REQUEST['export_links']) && $_REQUEST['export_links'] == 'on') ? true : false;
      $this->choices['update urls'] = (isset($_REQUEST['export_update_urls']) && $_REQUEST['export_update_urls'] == 'on') ? true : false;
      $all_ids = $this->obj_set->getAllObjectIDs();
      foreach($all_ids as $i)
      {
         if(isset($_REQUEST['export_obj_'.$i]) && $_REQUEST['export_obj_'.$i] == 'on')
         {
            $this->choices['obj'][] = $i;
         }
      }
      if(isset($_REQUEST['export_stories']) && trim($_REQUEST['export_stories']) != "")
      {
         $this->choices['stories'][0] = trim($_REQUEST['export_stories']);
         $this->choices['stories'][1] = (isset($_REQUEST['export_comments']) && $_REQUEST['export_comments'] == 'on') ? true : false;
         $this->choices['stories'][2] = (isset($_REQUEST['export_attachments']) && $_REQUEST['export_attachments'] == 'on') ? true : false;
      }
      if(isset($_REQUEST['target_collection']) and $_REQUEST['target_collection'] != "")
      {
         $this->choices['target'][0] = $_REQUEST['target_collection'];
         $this->choices['target'][1] = false;
      }
      elseif(isset($_REQUEST['new_collection']) and $_REQUEST['new_collection'] != "")
      {
         $this->choices['target'][0] = $_REQUEST['new_collection'];
         $this->choices['target'][1] = true;
      }
   }


   function checkUserInput()
   {
      if($this->metadata->name == "")
      {
         $this->user_error = "You must supply an id for this export";
         return false;
      }
      if(!preg_match('#\w{2,}#', $this->metadata->name))
      {
         $this->metadata->name = "";
         $this->user_error = "The id can only consist of letters, numbers and underscores";
         return false;
      }
      if($this->metadata->getMeta('description') == "")
      {
         $this->user_error = "You must supply a description of the exported collection";
         return false;
      }
      if($this->metadata->getMeta('title') == "")
      {
         $this->user_error = "You must supply a title of the exported collection";
         return false;
      }
      if(isset($this->choices['stories'][0]) and trim($this->choices['stories'][0]) != "")
      {
         if($this->getStoryCount() === false)
         {
            $this->user_error .= " There was an error in your sql input to select stories to export.";
            return false;
         }
      }
      return true;
   }


   function writeHiddenChoices()
   {
      echo "<input type='hidden' name='export_id' value='".htmlspecialchars($this->metadata->name, ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='export_description' value='".htmlspecialchars($this->metadata->getMeta("description"), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='export_title' value='".htmlspecialchars($this->metadata->getMeta("title"), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='export_icon_small' value='".htmlspecialchars($this->metadata->getMeta("iconsmall"), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='export_icon_medium' value='".htmlspecialchars($this->metadata->getMeta("iconmedium"), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='export_icon_large' value='".htmlspecialchars($this->metadata->getMeta("iconlarge"), ENT_QUOTES)."'>\n";
      $fs = array("language", "region","topic","type", "role", "obj");
      foreach($fs as $f)
      {
         foreach($this->choices[$f] as $e)
         {
            echo "<input type='hidden' name='export_".$f."_".$e."' value='on'>\n";
         }
      }
      if(isset($this->choices['stories'][0]))
      {
          echo "<input type='hidden' name='export_stories' value='".$this->choices['stories'][0]."'>\n";
          if($this->choices['stories'][1]) echo "<input type='hidden' name='export_comments' value='on'>\n";
          if($this->choices['stories'][2]) echo "<input type='hidden' name='export_attachments' value='on'>\n";
      }
      if($this->choices['redirects'])
      {
          echo "<input type='hidden' name='export_redirects' value='on'>\n";
      }
      if($this->choices['css'])
      {
          echo "<input type='hidden' name='export_css' value='on'>\n";
      }
      if($this->choices['links'])
      {
          echo "<input type='hidden' name='export_links' value='on'>\n";
      }
      if($this->choices['files'])
      {
         echo "<input type='hidden' name='export_files' value='on'>\n";
      }
      if($this->choices['update urls'])
      {
         echo "<input type='hidden' name='export_update_urls' value='on'>\n";
      }
  }

   function writeExportForm($cols, $can_update=true, $from_collection = null)
   {
      if($from_collection !== null)
      {
         $this->readChoicesFromCollection($from_collection);
      }
      $this->writeMetaDataSection($cols, $can_update);
      writeMenuSpacer();
      $this->writeCategorySection($cols, $can_update);
      writeMenuSpacer();
      $this->writeContentSection($cols, $can_update);
      $this->writeObjectSection($cols+1, $can_update);
      if($can_update)
      {
         $this->writeExportSubmit($cols, $can_update);
      }
   }

   function getStoryCount()
   {
      global $prefix, $dbconn;
      $sql = "SELECT count(story_id) from ".$prefix."_stories where ".$this->choices['stories'][0];
      $result = sql_query($sql, $dbconn, 2);
      if(!checkForError($result))
      {
         $this->user_error = mysql_errno()." - ".mysql_error();
         return false;
      }
      else
      {
         list($num) = sql_fetch_row($result);
      }
      return $num;
   }

   function writeMetaDataSection($cols, $can_update)
   {
      if($can_update)
      {
          $en = "";
      }
      else $en = "disabled";
      writeMenuSectionHeader("Collection Basic Information", "The basic information about this data collection", $cols);
      echo "<tr class='admin'><td class='admin' style='vertical-align: middle'>ID<br><small class='error'>(required)</small></td><td class='admin'>";
      echo "A short text string that identifies the collection (no spaces / funny characters)<br>";
      echo "<small>This will be used in the generated exported xml filename</small> <br>";
      echo "<input type='text' $en name='export_id' value='";
      echo htmlspecialchars($this->metadata->name, ENT_QUOTES);
      echo "' maxlength='32' size='32'></td>";
      echo "</tr>";
      echo "<tr class='admin'><td class='admin' style='vertical-align: middle'>Title <br><small class='error'>(required)</small></td><td class='admin'>";
      echo "The full title of this collection<br>";
      echo "<input type='text' $en name='export_title' maxlength='200' size='64' value='";
      echo htmlspecialchars($this->metadata->getMeta("title"), ENT_QUOTES);
      echo "'></td>";
      echo "</tr>";
      echo "<tr class='admin'><td class='admin' style='vertical-align: middle'>Description<br><small class='error'>(required)</small></td><td class='admin'>";
      echo "The textual description of this collection<br>";
      echo "<textarea $en name='export_description' cols='400' rows='3'>";
      echo htmlspecialchars($this->metadata->getMeta("description"), ENT_QUOTES);
      echo "</textarea></td>";
      echo "</tr>";
      echo "<tr class='admin'><td class='admin' style='vertical-align: middle'>Icons</td><td class='admin'>";
      echo "An Icon to accompany the collection (32x32 pixels)<br>";
      echo "<input type='text' $en name='export_icon_small' maxlength='200' size='64' value='";
      echo htmlspecialchars($this->metadata->getMeta("iconsmall"), ENT_QUOTES);
      echo "'></td>";
      echo "</tr>";
      writeMenuFooter($cols, false);
   }

   function writeExportSubmit($cols)
   {
      writeMenuSpacer();
      writeMenuHeader();

      echo "<tr class='admin'><td class='admin' colspan='$cols' align=center><input type='submit' name='save' value='Export &gt;&gt;'>";
      echo "</th></tr>";
   }

   function writeObjectSection($cols, $can_update=true)
   {
      if($can_update)
      {
         $en = "";
      }
      else $en = "disabled";
      $lang_code = $this->obj_set->calling_req->getObjectLanguage();
      $site_info = $this->obj_set->getObjectInfoByTypename("site", $lang_code);

      foreach($site_info as $s_id => $s_name)
      {
         $checkbox_array = array();
         writeMenuSpacer();
         writeMenuHeader();
         $c = "";
         if(isset($this->choices['obj']) && in_array($s_id, $this->choices['obj']))
            $c = 'checked';
         if($can_update)
         {
            $intro = "Choose which data objects from this site section to export";
            $title = "Export Modules from Site Section $s_name"." <input $en type='checkbox' name='export_obj_".$s_id.  "' $c>";
         }
         else
         {
            if(isset($this->choices['files_'.$s_id]))
            {
               $intro = $this->choices['files_'.$s_id]." files included with this section";
            }
            else
            {
               $intro = "no files included";
            }
            $title = "Included objects from Site Section $s_name";
         }
         writeMenuSectionHeader($title, $intro, $cols);
         $offset = 0;
         foreach($this->obj_set->supportedTypes->legalTypes as $t)
         {
            if(strtolower($t) == 'site') continue;
            $ret = $this->obj_set->getObjectInfoByTypename($t, $lang_code, $s_id);
            if(count($ret) > 0)
            {
               if($offset % $cols == 0)
               {
                  echo "<tr class='admin'>";
               }
               $offset++;
               echo "<td class='admin'>";
               echo "<h4 class='object-name-list'>$t Module</h4>";
               echo "<ul class='object-name-list'>";
               foreach($ret as $k => $v)
               {
                  $c = "";
                  if(isset($this->choices['obj']) && in_array($k, $this->choices['obj']))
                     $c = 'checked';

                  echo "<li class='object-name-list'><input $en type='checkbox' name='export_obj_".$k.  "' $c>$v <span class='obj_id'>object $k</span></li>";
                  $checkbox_array[] = "export_obj_".$k;
               }
               echo "</ul>";
            }
            echo "</td>";
            if($offset % $cols == 0)
            {
              echo "</tr>";
            }
         }
         if($offset % $cols != 0)
         {
            while($offset % $cols != 0)
            {
               echo "<td class='admin'>&nbsp;</td>";
               $offset++;
            }
            echo "</tr>";
         }
         if($offset == 0)
         {
            echo "<tr class='admin'><td class='admin' colspan='$cols'>No Objects set in this site section</td></tr>";
         }
	 if (count($checkbox_array) > 0 ) {
	     echo "<tr class='admin'><td class='admin'> <h4 class='object-name-list'>All Modules</h4> ";
             echo "<input $en type='checkbox' name='export_objects_".$s_id."_all' $c onClick='check_all_objects_".$s_id."()'> Tick All";
             echo "</td><td class='admin'>&nbsp;</td>";
             echo "<td class='admin'>&nbsp;</td>";
	     echo "</td></tr>";
             $this->writeJSFunction("check_all_objects_".$s_id, "export_objects_".$s_id."_all", $checkbox_array);
	 }
         writeMenuFooter($cols, false);
      }
   }

   function writeCategorySection($cols, $can_update=true)
   {
      if($can_update)
      {
         $en = "";
      }
      else $en = "disabled";
      writeMenuHeader();
      writeMenuSectionHeader("Basic Categories To Export", "Choose what content from the database will be exported", $cols);
      global $languageList, $topicList, $typeList, $regionList, $roleList;
      global $userLanguage, $pageLanguage;

      // Display the list of topics, regions and types in the displayed language.
      if (isset($userLanguage)) { $t_lang_id = $userLanguage->language_id; }
      else if (isset($pageLanguage)) { $t_lang_id = $pageLanguage->language_id; }
      else { $t_lang_id = $languageList->getMinLanguageId(); }

      $checkbox_array = array();
      $cat = 'language';
      echo "<tr class='admin'><td class='admin'>Languages to export</td><td class='admin'>";
      $langs = $languageList->getfLanguages();
      foreach($langs as $l)
      {
         if(isset($this->choices[$cat]) && in_array($l->language_id, $this->choices[$cat]))
         {
            $c = 'checked';
         }
         else
         {
            $c = "";
         }
         echo "<input type='checkbox' name='export_language_".$l->language_id."' $c $en> ".$l->language_name." ";
         $checkbox_array[] = "export_language_".$l->language_id;
      }
      if(isset($this->data_collection->new_elements[$cat]))
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getLanguageById($l_id);
            echo "<span style='color: #a88;'><input type='checkbox' name='export_null_".$l_id."' disabled> ".$t->language_name." (not present) </span>";
         }
      }
      echo "</td></tr>";
      echo "<tr class='admin'><td class='admin'>Types to export</td><td class='admin'>";
      $cat = 'type';
      $types = $typeList->getfTypes($t_lang_id);
      foreach($types as $t)
      {
         if(isset($this->choices[$cat]) && in_array($t->type_id, $this->choices[$cat]))
         {
            $c = 'checked';
         }
         else
         {
            $c = "";
         }
         echo "<input type='checkbox' name='export_type_".$t->type_id."' $c $en> ".$t->type_name." ";
         $checkbox_array[] = "export_type_".$t->type_id;
      }
      if(isset($this->data_collection->new_elements[$cat]))
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getTypeById($l_id, $t_lang_id);
            echo "<span style='color: #a88;'><input type='checkbox' name='export_null_".$l_id."' disabled> ".$t->type_name." (not present) </span>";
         }
      }
      echo "</td></tr>";
      echo "<tr class='admin'><td class='admin'>Topics to export</td><td class='admin'>";
      $ts = $topicList->getfTopics($t_lang_id);
      $cat = 'topic';
      foreach($ts as $t)
      {
         if(isset($this->choices['topic']) && in_array($t->topic_id, $this->choices['topic']))
         {
            $c = 'checked';
         }
         else
         {
            $c = "";
         }
         echo "<input type='checkbox' name='export_topic_".$t->topic_id."' $c $en> ".$t->topic_name." ";
         $checkbox_array[] = "export_topic_".$t->topic_id;
      }
      if(isset($this->data_collection->new_elements[$cat]))
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getTopicById($l_id, $t_lang_id);
            echo "<span style='color: #a88;'><input type='checkbox' name='export_null_".$l_id."' disabled> ".$t->topic_name." (not present) </span>";
         }
      }
      echo "</td></tr>";
      echo "<tr class='admin'><td class='admin'>Regions to export</td><td class='admin'>";
      $ts = $regionList->getfRegions($t_lang_id);
      $cat = 'region';
      foreach($ts as $t)
      {
         if(isset($this->choices['region']) && in_array($t->region_id, $this->choices['region']))
         {
            $c = 'checked';
         }
         else
         {
            $c = "";
         }
         echo "<input $en type='checkbox' name='export_region_".$t->region_id."' $c> ".$t->region_name." ";
         $checkbox_array[] = "export_region_".$t->region_id;
      }
      if(isset($this->data_collection->new_elements[$cat]))
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getRegionById($l_id, $t_lang_id);
            echo "<span style='color: #a88;'><input type='checkbox' name='export_null_".$l_id."' disabled> ".$t->region_name." (not present) </span>";
         }
      }
      echo "<tr class='admin'><td class='admin'>Roles to export</td><td class='admin'>";
      $cat = 'role';
      $ts = $roleList->getRoles();
      foreach($ts as $t)
      {
         if(isset($this->choices['role']) && in_array($t->role_id, $this->choices['role']))
         {
            $c = 'checked';
         }
         else
         {
            $c = "";
         }
         echo "<input $en type='checkbox' name='export_role_".$t->role_id."' $c> ".$t->role_name." ";
         $checkbox_array[] = "export_role_".$t->role_id;
      }
      if(isset($this->data_collection->new_elements[$cat]))
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getRoleById($l_id);
            echo "<span style='color: #a88;'><input type='checkbox' name='export_null_".$l_id."' disabled> ".$t->role_name." (not present) </span>";
         }
      }

      echo "<tr class='admin'><td class='admin'>Tick All </td><td class='admin'>";
      echo "<input $en type='checkbox' name='export_category_all' $c onClick='check_all_category()'> All";
      $this->writeJSFunction("check_all_category", "export_category_all", $checkbox_array);

      writeMenuFooter($cols, false);
   }
   function writeJSFunction($js_fn_name, $all_checkbox, $checkboxes)
   {
      ?> <script type="text/javascript" language="JavaScript">
      <?
      echo "function ".$js_fn_name."() 
      {
          if (document.exportform.".$all_checkbox.".checked == true )
	  {
      ";
      foreach ($checkboxes as $t_key => $t_value) {
  	      echo " document.exportform.".$t_value.".checked=true;\n";
      }
      echo "} else { \n"; 
      foreach ($checkboxes as $t_key => $t_value) {
  	     echo " document.exportform.".$t_value.".checked=false;\n";
      }
      ?>
          }
      }
      </script>
      <?
   }

   function writeContentSection($cols, $can_update=true)
   {
      if($can_update)
      {
         $en = "";
      }
      else $en = "disabled";
      writeMenuHeader();
      if($can_update)
      {
         writeMenuSectionHeader("Content To Export",  "Choose what content from the database will be exported", $cols);
      }
      else
      {
         writeMenuSectionHeader("Included Content",  "Content that was exported from the database.", $cols);
      }
      $checkbox_array = array("export_css","export_redirects");
      echo "<tr class='admin'><td class='admin'>Bits and bobs</td><td class='admin'>";
      $c = "";
      if(isset($this->choices['css']) && $this->choices['css'] == true) $c = 'checked';
      echo "<input $en type='checkbox' name='export_css' $c> Export Style Sheets? ";

      $c = "";
      if(isset($this->choices['redirects']) && $this->choices['redirects'] == true) $c = 'checked';
      echo "<input $en type='checkbox' name='export_redirects' $c> Export Friendly URLs? ";
      echo "</tr>";

      if($can_update)
      {
         array_unshift($checkbox_array,"export_links","export_files","export_update_urls");
         echo "<tr class='admin'><td class='admin'>Export Files</td><td class='admin'>";
         $c = "";
         if(isset($this->choices['links']) && $this->choices['links'] == true) $c = 'checked';
         echo "<input $en type='checkbox' name='export_links' $c> Export Local Files Linked by Objects ";
         $c = "";
         if(isset($this->choices['files']) && $this->choices['files'] == true) $c = 'checked';
         echo "<input $en type='checkbox' name='export_files' $c> Export All Collection Files ";
         echo "<tr class='admin'><td class='admin'>Map Links</td><td class='admin'>";
         $c = "";
         if(isset($this->choices['update urls']) && $this->choices['update urls'] == true) $c = 'checked';
         echo "<input $en type='checkbox' name='export_update_urls' $c> Update Embedded URLs ";
         echo "</td></tr>";
      }
      else // just display a summary...
      {
      }

      $c = "";

      if($can_update)
      {
         array_unshift($checkbox_array, "export_stories","export_comments", "export_attachments");
         echo "<tr class='admin'><td class='admin'>Stories select statement</td><td class='admin'>";
         echo "The SQL statement to select stories to export (leave blank to export no stories)<br>";
         echo "<textarea $en name='export_stories' cols='400' rows='3'></textarea></td>";
         echo "</tr>";
         echo "<tr class='admin'><td class='admin'>Stories export attachments</td><td class='admin'>";
         echo "<input $en type='checkbox' name='export_comments' $c> Include Comments ";
         echo "<input $en type='checkbox' name='export_attachments' $c> Include Attachments ";
         echo "</td></tr>";
      }
      else
      {
         echo "<tr class='admin'><td class='admin'>Stories select statement</td><td class='admin'>";
         echo "The SQL statement used to select stories to export ";
         $item = $this->metadata->getItemByName("content");
         if($item)
         {
            $sql = $item->getMeta("content_sql");
         }
         else $sql = "n/a";
         echo "<br>";
         echo "<textarea $en name='export_stories' cols='400' rows='3'>$sql</textarea></td>";
         echo "</tr>";
         //we just want a count don't we...
         echo "<tr class='admin'><td class='admin'>Content included</td><td class='admin'>";
         echo "Stories: ";
         if(isset($this->choices['stories']))
         {
            echo $this->choices['stories'];
         }
         else echo "0";
         echo " Comments: ";
         if(isset($this->choices['comments']))
         {
            echo $this->choices['comments'];
         }
         else echo "0";
         echo " Attachments: ";
         if(isset($this->choices['attachments']))
         {
            echo $this->choices['attachments'];
         }
         else echo "0";
         echo " Attachment files: ";
         if(isset($this->choices['attachment_files']))
         {
            echo $this->choices['attachment_files'];
         }
         else echo "0";
         echo "</td></tr>";
         //number of stories , number of comments, number of attachments, number of attachment files
      }
      echo "<tr class='admin'><td class='admin'>Tick All </td><td class='admin'>";
      echo "<input $en type='checkbox' name='export_content_all' $c onClick='check_all_content()'> All";
      echo "</td></tr>";
      $this->writeJSFunction("check_all_content", "export_content_all", $checkbox_array);
      writeMenuFooter($cols, false);
   }

   function compileMetadata()
   {
       $s = isset($this->choices['stories'][0]) ? $this->choices['stories'][0] : "";
       $this->metadata->setMeta("export_story_sql", $s);
   }


   function export()
   {
      global $system_config;
      $export_dir = $this->root_dir.$this->metadata->name;
      if(!is_dir($export_dir) && !mkdir($export_dir, $system_config->default_writable_directory_permissions))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $export_dir for export", "sf5"));
         return false;
      }
      else
      {
         touch($export_dir."/".$this->metadata->name.".index");
         if($system_config->debug_level >= DEBUG_LEVEL_1)
         {
             $this->setUserMessage(array(DEBUG_LEVEL_1, "Created Directory $export_dir",   "a3d5"));
         }
      }

      $this->exportCategories();
      $this->exportObjects();
      $this->exportSundries();
      if(isset($this->choices['stories'][0]) && $this->choices['stories'][0] != "")
      {
         $this->exportStories();
      }
      $this->metadata->save($this->root_dir);
      return true;
   }

   function exportCategories()
   {
      global $languageList, $topicList, $regionList, $typeList, $roleList, $system_config;
      // Make sure language is first in this array, as dumping of others depends on it.
      $cats = array("language", "topic", "region", "type", "role", "stories", "comments", "attachments", "attachments/files");
      $export_base = $this->root_dir.$this->metadata->name;
      foreach($cats as $c)
      {
         $export_dir = $export_base."/".$c;
         if(!is_dir($export_dir))
         {
            if(!mkdir($export_dir, $system_config->default_writable_directory_permissions))
            {
               $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $export_dir for export", "sf5"));
               return false;
            }
            else
            {
               if($system_config->debug_level >= DEBUG_LEVEL_1)
               {
                  $this->setUserMessage(array(
                     DEBUG_LEVEL_1, "Created Directory $export_dir",   "a3d5"));
               }
            }
         }
         if($c == "stories" or $c == "comments" or $c == 'obj' or $c == "attachments" or $c == "sundries" or $c == "attachments/files") continue;
         $values = $this->choices[$c];
         if($c == 'language')
         {
            $l_codes = array();
            $lang_ids = $values;
            foreach($values as $v)
            {
               $l = $languageList->getLanguageByID($v);
               $l_codes[] = $l->getLanguageCodePrefix();
            }
            $i = new indyDataItem($c, "categorylist");
            $i->setMeta("title", "Included Languages");
            $i->setMeta("entries", implode(",",$l_codes));
            $this->metadata->addItem($i);
            $languageList->dump($export_dir, $values);
         }
         elseif($c == 'region')
         {
            $r_codes = array();
            foreach($values as $v)
            {
               $r = $regionList->getRegionByID($v);
               $r_codes[] = $r->region_name;
            }
            $i = new indyDataItem($c, "categorylist");
            $i->setMeta("title", "Included Regions");
            $i->setMeta("entries", implode(",",$r_codes));
            $this->metadata->addItem($i);
            $regionList->dump($export_dir, $values);
         }
         elseif($c == 'topic')
         {
            $t_codes = array();
            foreach($values as $v)
            {
               $t = $topicList->getTopicByID($v);
               $t_codes[] = $t->topic_name;
            }
            $i = new indyDataItem($c, "categorylist");
            $i->setMeta("title", "Included Topics");
            $i->setMeta("entries", implode(",",$t_codes));
            $this->metadata->addItem($i);
            $topicList->dump($export_dir, $values, $lang_ids);
         }
         elseif($c == 'type')
         {
            $t_codes = array();
            foreach($values as $v)
            {
               $t = $typeList->getTypeByID($v);
               $t_codes[] = $t->type_name;
            }
            $i = new indyDataItem($c, "categorylist");
            $i->setMeta("title", "Included Types");
            $i->setMeta("entries", implode(",",$t_codes));
            $this->metadata->addItem($i);
            $typeList->dump($export_dir, $values);
         }
         elseif($c == 'role')
         {
            $r_codes = array();
            foreach($values as $v)
            {
               $r = $roleList->getRoleByID($v);
               $r_codes[] = $r->role_name;
            }
            $i = new indyDataItem($c, "categorylist");
            $i->setMeta("title", "Included Roles");
            $i->setMeta("entries", implode(",",$r_codes));
            $this->metadata->addItem($i);
            $roleList->dump($export_dir, $values);
         }
      }
   }

   function exportStories()
   {
      global $dbconn, $prefix;
      $export_base = $this->root_dir.$this->metadata->name;
      $links = $this->choices['update urls'];
      $comments = $this->choices['stories'][1];
      $attachments = $this->choices['stories'][2];
      $sql = "SELECT story_id from ".$prefix."_stories where ".$this->choices['stories'][0];
      $result = sql_query($sql, $dbconn, 2);
      if(!checkForError($result)) return false;
      if(sql_num_rows( $result ) == 0) return;
      for ($i=0; $i<sql_num_rows( $result ); $i++)
      {
         list($s_id) = sql_fetch_row($result, $dbconn);
         $s_ids[] = $s_id;
      }
      $comment_count = 0;
      $attachment_count = 0;
      foreach($s_ids as $s_id)
      {
         $s = new Story();
         $s->story_id = $s_id;
         $s->load();
         if($attachments)
         {
            $s->loadAttachments();
            foreach($s->attachments as $a)
            {
               $attachment_count++;
               $this->copyAttachmentFiles($a, $export_base);
               $dir_offset = ((int)($a->attachment_id / 1000)) + 1;
               $dump_target = $export_base."/attachments/".$dir_offset;
               $a->dump($dump_target, $links);
            }
         }
         if($comments)
         {
            $s->loadComments();
            foreach($s->comments as $c)
            {
               $comment_count++;
               if($attachments)
               {
                  $c->loadAttachments();
                  foreach($c->attachments as $a)
                  {
                     $attachment_count++;
                     $this->copyAttachmentFiles($a, $export_base);
                     $dir_offset = ((int)($a->attachment_id / 1000)) + 1;
                     $dump_target = $export_base."/attachments/".$dir_offset;
                     $a->dump($dump_target, $links);
                  }
               }
               $dir_offset = ((int)($c->comment_id / 1000)) + 1;
               $dump_target = $export_base."/comments/".$dir_offset;
               $c->dump($dump_target, $links);
            }
         }
         $dir_offset = ((int)($s_id / 1000)) + 1;
         $dump_target = $export_base."/stories/".$dir_offset;
         $s->dump($dump_target, $links);
      }
      $story_count = count($s_ids);
      $i = new indyDataItem("content", "categorylist");
      $i->setMeta("title", "Included Content");
      $i->setMeta("content_sql", $sql);
      $i->setMeta("stories", $story_count);
      $i->setMeta("comments", $comment_count);
      $i->setMeta("attachments", $attachment_count);
      $this->metadata->addItem($i);
      return true;
   }

   function copyAttachmentFiles(&$a, $export_base)
   {
      global $system_config;
      $orig_file = $system_config->attachment_store . $a->attachment_file;
      $attach_rel_path = "attachments/files/".substr($a->attachment_file, 0, 3);
      $file_target = $export_base."/".$attach_rel_path;
      if(!is_dir($file_target))
      {
         if(!mkdir($file_target, $system_config->default_writable_directory_permissions))
         {
            $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $file_target for export", "sfs"));
            return false;
         }
      }
      if(file_exists($orig_file))
      {
         $p = pathinfo($a->attachment_file);
         $fname = $file_target."/".$p['basename'];
         copy($orig_file, $fname);
         $a->attachment_file = $attach_rel_path."/".$p['basename'];
      }
      else echo "<P>file $a->attachment_file does not exist</p>";
   }

   function exportObjects()
   {
      $export_base = $this->root_dir.$this->metadata->name."/";
      $site_info = $this->obj_set->getObjectInfoByTypename("site", $this->obj_set->calling_req->getObjectLanguage());
      foreach($site_info as $k => $v)
      {
         if(in_array($k, $this->choices['obj']))
         {
            $this->obj_set->export($export_base, $k, $this->choices, $this->metadata);
         }
      }
   }

   function exportSundries()
   {
      global $pseudo_directory_offset, $system_config;
      $i = new indyDataItem("sundries", "categorylist");
      $i->setMeta("title", "Included Sundries");
      $export_base = $this->root_dir.$this->metadata->name."/";
      $site_info = $this->obj_set->getObjectInfoByTypename("site", $this->obj_set->calling_req->getObjectLanguage());
      foreach($site_info as $k => $v)
      {
         if(!in_array($k, $this->choices['obj'])) continue;
         $sundry_base = $export_base."site_section_".$k."/sundries/";
         if($this->choices['css'])
         {
            $i->setMeta("css", "true");
            if(!is_dir($sundry_base))
            {
               if(!mkdir($sundry_base, $system_config->default_writable_directory_permissions))
               {
                  $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $sundry_base for export", "sf5"));
                  return false;
               }
               else
               {
                  if($system_config->debug_level >= DEBUG_LEVEL_1)
                  {
                     $this->setUserMessage(array(
                        DEBUG_LEVEL_1, "Created Directory $sundry_base",   "a3d5"));
                  }
               }
            }

            if(in_array($k, $this->choices['obj']))
            {
               $site_obj = $this->obj_set->fetchObject($k, $this->obj_set->calling_req->getObjectLanguage());
               if($site_obj->getMeta('different_css') == 'on')
               {
                  $st_store = $system_config->site_file_store_base.'site_'.$k.'/';
               }
               else
               {
                  $st_store = $system_config->site_file_store_base.'default/';
               }
               $st_store = $pseudo_directory_offset.$st_store;
               $a = array("common", "style", "contentstyle");
               foreach($a as $s)
               {
                  if(file_exists($st_store.$s.".css"))
                  {
                     copy($st_store.$s.".css", $sundry_base.$s.".css");
                  }
                  else
                  {
                     echo "<P>File $st_store"."$s.css not found</p>";
                  }
               }
            }
         }
         if($this->choices['files'])
         {
            $file_count = 0;
            $st_store = $system_config->site_file_store_base.'site_'.$k.'/';
            $dh=opendir($st_store);
            if(!$dh)
            {
               reportError("Couldn't open export directory: ".$st_store);
            }
            while($file=readdir($dh))
            {
               if(!is_dir($st_store."/".$file))
               {
                  copy($st_store."/".$file, $sundry_base.$file);
                  $file_count++;
               }
               else
               {
                  continue;
               }
            }
            closedir($dh);
            $i->setMeta("files", $file_count);
         }
      }
      if($this->choices['redirects'])
      {
         global $redirectList;
         $i->setMeta("redirects", "true");
         $redirectList->load();
         $new_redirect = new RedirectList();
         $target_file = $export_base . "redirector.inc";
         if(file_exists($target_file))
         {
            $new_redirect->load(true, $target_file);
         }
         foreach($this->choices['obj'] as $id)
         {
            $copy_red = $redirectList->getRedirect($id);
            if($copy_red !== false)
            {
               $new_redirect->add($copy_red);
            }
         }
         $new_redirect->save($target_file);
      }
      $this->metadata->addItem($i);
   }

} 
?>
