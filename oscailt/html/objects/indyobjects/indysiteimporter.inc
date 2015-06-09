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

class indySiteImporter extends indyImportExport
{
   var $import_source_map;
   var $import_id;
   var $basic_choices;
   var $loaded_from_form = false;
   var $is_new_install = false;
   var $import_record = array();

   function indySiteImporter(&$obj_set)
   {
      global $system_config;
      $this->indyImportExport($system_config->object_import_dir, $obj_set);
   }

   function initialliseImportMap()
   {
      $this->import_source_map = array(
            "language" => array(),
            "region" => array(),
            "topic" => array(),
            "type" => array(),
            "role" => array(),
            "obj" => array(),
            "stories" => array(),
            "comments" => array(),
            "attachments" => array(),
            "attachments_files" => array(),
            "css" => null,
            "links" => null,
            "files" => null,
            "redirects" => null,
            "update urls" => null);
   }

   function writeChoiceForm($cols, &$s_o, $target, $is_full=false)
   {
      $this->import_id = $target;
      if(!$this->loaded_from_form)
      {
         $this->readChoicesFromCollection($target);
      }
      else
      {
      }
      echo "<input type='hidden' name='target_collection' value='$this->import_id'>";
      if(!$is_full)
      {
         echo "<input type='hidden' name='action' value='import'>";
         $this->writeChooseCollectionSection($cols, $s_o, "import", $target);
         writeMenuSpacer();
         $this->writeBasicChoicesSection($s_o, $is_full);
      }
      else
      {
         echo "<input type='hidden' name='action' value='import-choose'>";
         $this->writeMetaDataSection($cols);
         writeMenuSpacer();
         $this->writeChooseCategoriesSection(3, $s_o);
         writeMenuSpacer();
         $this->writeContentSection(3, $s_o);
         writeMenuSpacer();
         $lang_code = $this->obj_set->calling_req->getObjectLanguage();
         $site_info = $this->data_collection->obj_set->getObjectInfoByTypename("site", $lang_code);

         foreach($site_info as $s_id => $s_name)
         {
            $this->writeSiteSection(2, $s_o, $s_name, $s_id);
            writeMenuSpacer();
         }
      }
      writeMenuHeader($cols);
      $button_html = "<input type='submit' name='save' value='Import ".$this->import_id." Collection &gt;&gt;'>";
      writeMenuFooter($cols, false, $button_html);
      writeMenuSpacer();
   }

   function readBasicChoices()
   {
      if(!isset($_REQUEST['target_collection'])) return false;
      $this->import_id = $_REQUEST['target_collection'];
      $this->readChoicesFromCollection($_REQUEST['target_collection']);
      $import_sites = $this->data_collection->obj_set->getObjectStubsByTypename("site");
      if(isset($_REQUEST['renumber'])&& $_REQUEST['renumber'] == 'on')
      {
         $this->basic_choices['renumber'] = true;
      }
      foreach($import_sites as $s)
      {
         if(isset($_REQUEST['section_import_map_'.$s->obj_id]))
         {
             $this->choices['section_import_map'][$s->obj_id] = $_REQUEST['section_import_map_'.$s->obj_id];
         }
         else
         {
             $this->choices['section_import_map'][$s->obj_id] = 0;
         }
      }
      $this->loaded_from_form = true;
   }

   function readFullChoices()
   {
      global $languageList, $topicList, $typeList, $regionList, $roleList;
      if(!isset($_REQUEST['target_collection'])) return false;
      $this->import_id = $_REQUEST['target_collection'];
      $this->readChoicesFromCollection($_REQUEST['target_collection']);
      $this->loaded_from_form = true;
      $this->clearChoices();
      $cat = 'language';
      $l_list = $this->data_collection->entries[$cat];
      $langs = $languageList->getLanguages();

      foreach($langs as $l)
      {
         if(isset($_REQUEST['import_language_'.$l->language_id]) && $_REQUEST['import_language_'.$l->language_id] == 'on')
         {
            $this->choices[$cat][] = $l->language_id;
         }
      }
      $langs = $l_list->getLanguages();
      foreach($langs as $l)
      {
         if(isset($_REQUEST['import_new_language_'.$l->language_id]) && $_REQUEST['import_new_language_'.$l->language_id] == 'on')
         {
            $this->choices["new_".$cat][] = $l->language_id;
         }
      }

      $cat = 'type';
      $l_list = $this->data_collection->entries[$cat];
      $types = $typeList->getTypes();
      foreach($types as $t)
      {
         if(isset($_REQUEST['import_type_'.$t->type_id]) && $_REQUEST['import_type_'.$t->type_id] == 'on')
         {
            $this->choices[$cat][] = $t->type_id;
         }
      }
      $types = $l_list->getTypes();
      foreach($types as $t)
      {
         if(isset($_REQUEST['import_new_type_'.$t->type_id]) && $_REQUEST['import_new_type_'.$t->type_id] == 'on')
         {
            $this->choices["new_".$cat][] = $t->type_id;
         }
      }

      $cat = 'topic';
      $l_list = $this->data_collection->entries[$cat];
      $topics = $topicList->getTopics();
      foreach($topics as $t)
      {
         if(isset($_REQUEST['import_topic_'.$t->topic_id]) && $_REQUEST['import_topic_'.$t->topic_id] == 'on')
         {
            $this->choices[$cat][] = $t->topic_id;
         }
      }
      $topics = $l_list->getTopics();
      foreach($topics as $t)
      {
         if(isset($_REQUEST['import_new_topic_'.$t->topic_id]) && $_REQUEST['import_new_topic_'.$t->topic_id] == 'on')
         {
            $this->choices["new_".$cat][] = $t->topic_id;
         }
      }

      $cat = 'region';
      $l_list = $this->data_collection->entries[$cat];
      $regions = $regionList->getRegions();
      foreach($regions as $r)
      {
         if(isset($_REQUEST['import_region_'.$r->region_id]) && $_REQUEST['import_region_'.$r->region_id] == 'on')
         {
            $this->choices[$cat][] = $r->region_id;
         }
      }
      $regions = $l_list->getRegions();
      foreach($regions as $r)
      {
         if(isset($_REQUEST['import_new_region_'.$r->region_id]) && $_REQUEST['import_new_region_'.$r->region_id] == 'on')
         {
            $this->choices["new_".$cat][] = $r->region_id;
         }
      }

      $cat = 'role';
      $l_list = $this->data_collection->entries[$cat];
      $rs = $roleList->getRoles();
      foreach($rs as $r)
      {
         if(isset($_REQUEST['import_role_'.$r->role_id]) && $_REQUEST['import_role_'.$r->role_id] == 'on')
         {
            $this->choices[$cat][] = $r->role_id;
         }

      }
      $rs = $l_list->getRoles();
      foreach($rs as $r)
      {
         if(isset($_REQUEST['import_new_role_'.$r->role_id]) && $_REQUEST['import_new_role_'.$r->role_id] == 'on')
         {
            $this->choices["new_".$cat][] = $r->role_id;
         }

      }
      if(isset($_REQUEST['import_stories']) && $_REQUEST['import_stories'] == 'on')
      {
         $this->choices['stories'] = true;
      }
      if(isset($_REQUEST['import_comments']) && $_REQUEST['import_comments']  == 'on')
      {
         $this->choices['comments'] = true;
      }
      if(isset($_REQUEST['import_attachments']) && $_REQUEST['import_attachments']  == 'on')
      {
         $this->choices['attachments'] = true;
      }
      if(isset($_REQUEST['import_redirects']) && $_REQUEST['import_redirects']  == 'on')
      {
         $this->choices['redirects'] = true;
      }
      else
      {
         $this->choices['redirects'] = false;
      }
      $this->choices['import_css'] = array();
      $this->choices['import_files'] = array();
      $this->choices['import_renumber'] = array();
      $this->choices['section_import_map'] = array();
      $this->choices['obj_import_map'] = array();
      $import_sites = $this->data_collection->obj_set->getObjectsByTypename("SITE", $this->obj_set->calling_req->getObjectLanguage());
      foreach($import_sites as $site_obj)
      {
         if(isset($_REQUEST['import_obj_'.$site_obj->objref]) && $_REQUEST['import_obj_'.$site_obj->objref] == 'on')
         {
             $this->choices['obj'][] = $site_obj->objref;
             if(isset($_REQUEST['import_css_'.$site_obj->objref]) && $_REQUEST['import_css_'.$site_obj->objref] == 'on')
             {
               $this->choices['import_css'][$site_obj->objref] = true;
             }
             if(isset($_REQUEST['import_renumber_'.$site_obj->objref]) && $_REQUEST['import_renumber_'.$site_obj->objref] == 'on')
             {
               $this->choices['import_renumber'][$site_obj->objref] = true;
             }
             if(isset($_REQUEST['import_files_'.$site_obj->objref]) && $_REQUEST['import_files_'.$site_obj->objref] == 'all')
             {
               $this->choices['import_files'][$site_obj->objref] = "all";
             }
             elseif(isset($_REQUEST['import_files_'.$site_obj->objref]) && $_REQUEST['import_files_'.$site_obj->objref] == 'linked')
             {
               $this->choices['import_files'][$site_obj->objref] = "linked";
             }
             if(isset($_REQUEST['section_import_map_'.$site_obj->objref]))
             {
               $this->choices['section_import_map'][$site_obj->objref] = $_REQUEST['section_import_map_'.$site_obj->objref];
             }
             $all_objs = $this->data_collection->obj_set->getObjectsBySite($site_obj->objref);
             foreach($all_objs as $o)
             {
               if(isset($_REQUEST['import_obj_'.$o->obj_id]) && ($_REQUEST['import_obj_'.$o->obj_id] == 'on') && strtolower($o->obj_type) != "site")
               {
                  $this->choices['obj'][] = $o->obj_id;
               }
               if(isset($_REQUEST['obj_import_map_'.$o->obj_id]) && ($_REQUEST['obj_import_map_'.$o->obj_id]  == "on"))
               {
                  $this->choices['obj_import_map'][$o->obj_id] = true;
               }
             }
          }
      }
   }



   function writeHiddenChoices()
   {
      echo "<input type='hidden' name='target_collection' value='".htmlspecialchars($this->import_id)."'>\n";
      $fs = array("language","region","topic","type", "role", "obj");
      foreach($fs as $f)
      {
         foreach($this->choices[$f] as $e)
         {
            echo "<input type='hidden' name='import_".$f."_".$e."' value='on'>\n";
         }
         if(isset($this->choices["new_".$f]))
         {
            foreach($this->choices["new_".$f] as $e)
            {
               echo "<input type='hidden' name='import_new_".$f."_".$e."' value='on'>\n";
            }
         }
      }
      $fs = array("redirects", "stories", "comments", "attachments");
      foreach($fs as $f)
      {
         if($this->choices[$f])
         {
             echo "<input type='hidden' name='import_".$f."' value='on'>\n";
         }
      }
      $fs = array("import_css", "import_renumber", "obj_import_map");
      foreach($fs as $f)
      {
         foreach($this->choices[$f] as $k => $v)
         {
            echo "<input type='hidden' name='".$f."_".$k."' value='on'>\n";
         }
      }
      $fs = array ("section_import_map","import_files");
      foreach($fs as $f)
      {
         foreach($this->choices[$f] as $k => $v)
         {
            echo "<input type='hidden' name='".$f."_".$k."' value='".htmlspecialchars($v, ENT_QUOTES)."'>\n";
         }
      }
      ///op_r($this->choices);
   }


   function checkUserInput()
   {
      if(is_array($this->basic_choices))
      {
         return true;
      }
      if($this->import_id == null) return false;
      //op_r($this->choices);
      return true;

   }

   function writeSiteSection($cols, &$s_o, $s_name, $s_id)
   {
      writeMenuHeader();
      writeMenuSectionHeader($s_name." Section", "Choose data objects from this section to import", $cols);
      if(isset($this->choices['obj']) && in_array($s_id, $this->choices['obj']))
          $c = 'checked';
      else $c = "";
      echo "<tr class='admin'>
            <td class='admin'>Import Site Section</td>
            <td class='admin' colspan='";
      echo $cols-1;
      echo "'><input type='checkbox' name='import_obj_$s_id' $c><i><b>note</b>: if you do not check this box, all choices in this section will be ignored</i>.";
      echo "<br><br>Tick All <input type='checkbox' name='import_category_all_".$s_id."' $c onClick='check_all_site_".$s_id."()'> <i>to select or deselect all choices.";
      echo "</td></tr>";
      $checkbox_array = array();
      $this->writeJSFunction("check_all_site_".$s_id, "import_category_all_".$s_id, $checkbox_array);



      if(isset($this->choices['import_renumber'][$s_id])) $c = "checked";
      else $c = "";
      echo "<tr class='admin'>
            <td class='admin'>Renumber All Objects</td>
            <td class='admin' colspan='";
      echo $cols-1;

      echo "'><input type='checkbox' name='import_renumber_$s_id' $c> All imported objects from this section will be assigned new object IDs <i><b>note</b>: if you check this box, ID renumbering settings of individual objects will be ignored</i>.</td></tr>";
      echo "<tr class='admin'><td class='admin'>Choose Section to Import to</td>";
      echo "<td class='admin' colspan='";
      echo $cols-1;
      echo "'>";
      echo $this->getImportSectionMapTargetsAsSelect($s_id);
      echo "</td></tr>";
      echo "<tr class='admin'><td class='admin'>Import Style Sheets</td>";
      echo "<td class='admin' colspan='";
      echo $cols-1;
      echo "'>";
      if(isset($this->choices['import_css'][$s_id]) && $this->choices['import_css'][$s_id] == true)
      {
         echo "<input type='checkbox' name='import_css_$s_id' checked>";
	 $checkbox_array[] = "import_css_".$s_id;
      }
      elseif(isset($this->data_collection->entries['css']))
      {
         echo "<input type='checkbox' name='import_css_$s_id'>";
	 $checkbox_array[] = "import_css_".$s_id;
      }
      else
      {
         echo "<i>not available</i>";
      }
      echo "</td></tr>";
      echo "<tr class='admin'><td class='admin'>Import Section Files</td>";
      echo "<td class='admin' colspan='";
      echo $cols-1;
      echo "'>";
      if(isset($this->choices['import_files'][$s_id]))
      {
         if($this->choices['import_files'][$s_id] == 'all')
         {
            echo "<input type='radio' name='import_files_$s_id' value='all' checked> All Files ";
            echo "<input type='radio' name='import_files_$s_id' value='linked'> Embedded Files ";
            echo "<input type='radio' name='import_files_$s_id' value='none'> No Files ";
         }
         else
         {
            echo "<input type='radio' name='import_files_$s_id' value='all'> All Files ";
            echo "<input type='radio' name='import_files_$s_id' value='linked' checked> Embedded Files ";
            echo "<input type='radio' name='import_files_$s_id' value='none'> No Files ";
         }
      }
      else
      {
         echo "<input type='radio' name='import_files_$s_id' value='all'> All Files ";
         echo "<input type='radio' name='import_files_$s_id' value='linked'> Embedded Files ";
         echo "<input type='radio' name='import_files_$s_id' value='none'  checked> No Files ";
      }
      echo "</td></tr>";
      echo "</table>";
      echo "<table class='admin'>";

      $lang_code = $this->obj_set->calling_req->getObjectLanguage();
      $offset = 0;
      foreach($this->obj_set->supportedTypes->legalTypes as $t)
      {
         if(strtolower($t) == 'site') continue;
         $ret = $this->data_collection->obj_set->getObjectInfoByTypename($t, $lang_code, $s_id);
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
               $opts = $this->getObjectMappingChoices($k);
               $c = "";
               if(isset($this->choices['obj']) && in_array($k, $this->choices['obj']))
                  $c = 'checked';
               echo "<li class='object-name-list'><input    type='checkbox' name='import_obj_".$k.  "' $c>  $opts $v <span class='obj_id'>object $k</span></li>";
	       $checkbox_array[] = "import_obj_".$k;
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

      $this->writeJSFunction("check_all_site_".$s_id, "import_category_all_".$s_id, $checkbox_array);

      writeMenuFooter($cols, false);
   }

   function getObjectMappingChoices($obj_id)
   {
      //need to set this from choices
      if(isset($this->choices['obj_import_map'][$obj_id]) &&  $this->choices['obj_import_map'][$obj_id] == 'on')
      {
         $s = '';
         $s2 = 'selected';
      }
      else
      {
         $s = 'selected';
         $s2 = '';

      }
      $str = "<select class='object-map-list' name='obj_import_map_$obj_id'>";
      if($this->obj_set->containsObjectID($obj_id))
      {
         $str .= "<option value='' $s>Assign New Object ID</option>";
         $str .= "<option value='on' $s2>Overwrite Existing Object</option>";
      }
      else
      {
         $str .= "<option value='' $s>Assign New Object ID</option>";
         $str .= "<option value='on' $s2>Retain Existing ID</option>";
      }

      $str .= "</select>";
      return $str;
   }

   function getImportSectionMapTargetsAsSelect($target_id)
   {
      $installed_sites = $this->obj_set->getObjectInfoByTypename("site", $this->obj_set->calling_req->getObjectLanguage());
      $i_opts = "";
      foreach($installed_sites as $k => $v)
      {
         if(isset($this->choices['section_import_map'][$target_id]) && $k == $this->choices['section_import_map'][$target_id]) $s = 'selected';
         else $s = "";
         $i_opts .= "<option $s value='$k'>Import into $v</option>\n";
      }
      $map_select = "<select name='section_import_map_".$target_id."'>";
      $map_select .= "<option value='0'>Create As New Section</option>\n";
      $map_select .= $i_opts;
      $map_select .= "</select>";
      return $map_select;
   }

   function writeBasicChoicesSection(&$s_o, $is_full=false)
   {
      $import_sites = $this->data_collection->obj_set->getObjectsByTypename("SITE", $this->obj_set->calling_req->getObjectLanguage());
      $this->obj_set->setUserMessages($this->data_collection->obj_set->getUserMessages());
      $this->obj_set->writeUserMessageBox();
      writeMenuHeader();
      writeMenuSectionHeader("Import <i>$this->import_id</i> Collection", "Import the data collection listed below", 2);
     // echo "<tr class='admin'<td class='admin' style='vertical-align: middle'>Renumber Objects? <input type='checkbox' name='renumber'> Do you want to assign new ids to the objects that you are importing?</td></tr>";
      echo "<input type='hidden' name='renumber' value='on'>";
      echo "<tr class='admin'>";
      echo "<td class='admin' colspan='2' style='vertical-align: middle'>Map sections in the collection that you are importing to existing sections <br>";
      echo "</tr>";

      echo "<tr class='admin'><th class='admin'>Import Section Name</th><th class='admin'>Import into  Section</th></tr>";
      foreach($import_sites as $obj)
      {
         $map_select = $this->getImportSectionMapTargetsAsSelect($obj->objref);
         echo "<tr class='admin'>";
         echo "<td class='admin'>".$obj->name()."</td>";
         echo "<td class='admin'>$map_select</td>";
         echo "</tr>";
      }
   }


   function writeChooseCategoriesSection($cols, &$s_o)
   {
      $checkbox_array = array();
      writeMenuHeader();
      writeMenuSectionHeader("Basic Categories To Import", "Choose which of the basic categories from the database will be imported", $cols);
      global $languageList, $topicList, $typeList, $regionList, $roleList;
      $cat = 'language';
      echo "<tr class='admin'><th class='admin'>Category</th><th class='admin'>Installed (check to update)</th><th class='admin'>New (check to import)</th></tr>";
      echo "<tr class='admin'><td class='admin'>Languages</td><td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      $langs = $languageList->getLanguages();
      foreach($langs as $l)
      {
         if(isset($this->data_collection->installed_elements[$cat]) && in_array($l->language_id, $this->data_collection->installed_elements[$cat]))
         {
            $d = "";
            if($this->loaded_from_form && in_array($l->language_id, $this->choices[$cat]))$d = "checked";
            echo "<li><input type='checkbox' name='import_language_".$l->language_id."' $d> ".$l->language_name;
	    $checkbox_array[] = "import_language_".$l->language_id;
         }
         else
         {
            echo "<li><input type='checkbox' name='import_null_".$l->language_id."' disabled>".$l->language_name." (not in collection)";
         }
      }
      echo "</ul></td>";
      echo "<td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      if(isset($this->data_collection->new_elements[$cat]) && count($this->data_collection->new_elements[$cat]) > 0)
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            if(isset($this->choices["new_".$cat]) && in_array($l_id, $this->choices["new_".$cat]))
            {
               $c = 'checked';
            }
            else
            {
               $c = "";
            }
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getLanguageById($l_id);
            echo "<li><input type='checkbox' name='import_new_language_".$l_id."' $c> ".$t->language_name;
	    $checkbox_array[] = "import_new_language_".$l_id;
         }
      }
      else
      {
         echo "<i>No new $cat to install</i>";
      }
      echo "</ul></td></tr>";
      echo "<tr class='admin'><td class='admin'>Types</td><td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";

      $cat = 'type';
      $types = $typeList->getTypes();
      foreach($types as $t)
      {
         if(isset($this->data_collection->installed_elements[$cat]) && in_array($t->type_id, $this->data_collection->installed_elements[$cat]))
         {
            $d = "";
            if($this->loaded_from_form && in_array($t->type_id, $this->choices[$cat])) $d = "checked";
            echo "<li><input type='checkbox' name='import_type_".$t->type_id."' $d> ".$t->type_name;
	    $checkbox_array[] = "import_type_".$t->type_id;
         }
         else
         {
            echo "<li><input type='checkbox' name='import_null_".$t->type_id."' disabled>".$t->type_name." (not in collection)";
         }
      }
      echo "</ul></td>";
      echo "<td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      if(isset($this->data_collection->new_elements[$cat]) && count($this->data_collection->new_elements[$cat]) > 0)
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            if(isset($this->choices["new_".$cat]) && in_array($l_id, $this->choices["new_".$cat]))            {
               $c = 'checked';
            }
            else
            {
               $c = "";
            }
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getTypeById($l_id);
            echo "<li><input type='checkbox' name='import_new_type_".$l_id."' $c> ".$t->type_name;
	    $checkbox_array[] = "import_new_type_".$l_id;
         }
      }
      else
      {
         echo "<i>No new $cat to install</i>";
      }
      echo "</ul></td></tr>";
      echo "<tr class='admin'><td class='admin'>Topics</td><td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      $ts = $topicList->getTopics();
      $cat = 'topic';
      foreach($ts as $t)
      {
         if(isset($this->data_collection->installed_elements[$cat]) && in_array($t->topic_id, $this->data_collection->installed_elements[$cat]))
         {
            $d = "";
            if($this->loaded_from_form && in_array($t->topic_id, $this->choices[$cat])) $d = "checked";
            echo "<li><input type='checkbox' name='import_topic_".$t->topic_id."' $d> ".$t->topic_name;
	    $checkbox_array[] = "import_topic_".$t->topic_id;
         }
         else
         {
            echo "<li><input type='checkbox' name='import_null_".$t->topic_id."' disabled>".$t->topic_name." (not in collection)";
         }
      }
      echo "</ul></td>";
      echo "<td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      if(isset($this->data_collection->new_elements[$cat]) && count($this->data_collection->new_elements[$cat]) > 0)
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            if(isset($this->choices["new_".$cat]) && in_array($l_id, $this->choices["new_".$cat]))            {
               $c = 'checked';
            }
            else
            {
               $c = "";
            }
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getTopicById($l_id);
            echo "<li><input type='checkbox' name='import_new_topic_".$l_id."' $c> ".$t->topic_name;
	    $checkbox_array[] = "import_new_topic_".$l_id;
         }
      }
      else
      {
         echo "<i>No new $cat available to install</i>";
      }
      echo "</ul></td></tr>";
      echo "<tr class='admin'><td class='admin'>Regions</td><td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      $ts = $regionList->getRegions();
      $cat = 'region';
      foreach($ts as $t)
      {
         if(isset($this->data_collection->installed_elements[$cat]) && in_array($t->region_id, $this->data_collection->installed_elements[$cat]))
         {
            $d = "";
            if($this->loaded_from_form && in_array($t->region_id, $this->choices[$cat])) $d = "checked";
            echo "<li><input type='checkbox' name='import_region_".$t->region_id."' $d> ".$t->region_name;
	    $checkbox_array[] = "import_region_".$t->region_id;
         }
         else
         {
            echo "<li><input type='checkbox' name='import_null_".$t->region_id."' disabled>".$t->region_name." (not in collection)";
         }
      }
      echo "</ul></td>";
      echo "<td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      if(isset($this->data_collection->new_elements[$cat]) && count($this->data_collection->new_elements[$cat]) > 0)
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            if(isset($this->choices["new_".$cat]) && in_array($l_id, $this->choices["new_".$cat]))            {
               $c = 'checked';
            }
            else
            {
               $c = "";
            }
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getRegionById($l_id);
            echo "<li><input type='checkbox' name='import_new_region_".$l_id."' $c> ".$t->region_name;
	    $checkbox_array[] = "import_new_region_".$l_id;
         }
      }
      else
      {
         echo "<i>No new $cat to install</i>";
      }
      echo "</ul></td></tr>";
      $cat = 'role';
      echo "<tr class='admin'><td class='admin'>Roles</td><td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      $ts = $roleList->getRoles();
      foreach($ts as $t)
      {

         if(isset($this->data_collection->installed_elements[$cat]) && in_array($t->role_id, $this->data_collection->installed_elements[$cat]))
         {
            $d = "";
	    if($this->loaded_from_form && in_array($t->role_id, $this->choices[$cat])) $d = "checked";            
	    echo "<li><input type='checkbox' name='import_role_".$t->role_id."' $d> ".$t->role_name;
	    $checkbox_array[] = "import_role_".$t->role_id;
         }
         else
         {
            echo "<li><input type='checkbox' name='import_null_".$t->role_id."' disabled>".$t->role_name." (not in collection)";
         }
      }
      echo "</ul></td>";
      echo "<td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      if(isset($this->data_collection->new_elements[$cat]) && count($this->data_collection->new_elements[$cat]) > 0)
      {
         foreach($this->data_collection->new_elements[$cat] as $l_id)
         {
            if(isset($this->choices["new_".$cat]) && in_array($l_id, $this->choices["new_".$cat]))            {
               $c = 'checked';
            }
            else
            {
               $c = "";
            }
            $ts = $this->data_collection->entries[$cat];
            $t = $ts->getRoleById($l_id);
            echo "<li><input type='checkbox' name='import_new_role_".$l_id."' $c> ".$t->role_name;
	    $checkbox_array[] = "import_new_role_".$l_id;
         }
      }
      else
      {
         echo "<i>No new $cat to install</i>";
      }
      echo "</ul></td></tr>";

      echo "<tr class='admin'><td class='admin'>Tick All </td><td class='admin'>";
      echo "<input $en type='checkbox' name='import_category_all' $c onClick='check_all_category()'> All";
      echo "</td><td class='admin'> </td></tr>";
      $this->writeJSFunction("check_all_category", "import_category_all", $checkbox_array);
      writeMenuFooter($cols, false);
   }
   function writeJSFunction($js_fn_name, $all_checkbox, $checkboxes)
   {
      ?> <script type="text/javascript" language="JavaScript">
      <?
      echo "function ".$js_fn_name."() 
      {
          if (document.importform.".$all_checkbox.".checked == true )
	  {
      ";
      foreach ($checkboxes as $t_key => $t_value) {
  	      echo " document.importform.".$t_value.".checked=true;\n";
      }
      echo "} else { \n"; 
      foreach ($checkboxes as $t_key => $t_value) {
  	     echo " document.importform.".$t_value.".checked=false;\n";
      }
      ?>
          }
      }
      </script>
      <?
   }


   function writeContentSection($cols)
   {
      writeMenuHeader();
      writeMenuSectionHeader("Import Content", "Choose whether to import collection content or not", $cols);

      echo "<tr class='admin'><td class='admin'>Content Elements</td><td class='admin'>";
      echo "<ul class='object-name-list' style='margin-top: 0.1em;'>";
      if(isset($this->choices['stories']) && $this->choices['stories'] === true)
      {
         echo "<li><input type='checkbox' name='import_stories' checked> Import Stories ";
         echo "(".count($this->data_collection->entries['stories']).")";
      }
      elseif(isset($this->data_collection->entries['stories']) && count($this->data_collection->entries['stories']) === true)
      {
         echo "<li><input type='checkbox' name='import_stories'> Import Stories ";
         echo "(".count($this->data_collection->entries['stories']).")";
      }
      else
      {
         echo "<li><i>No stories</i>";
      }
      if(isset($this->choices['comments']) && $this->choices['comments'] === true)
      {
         echo "<li><input type='checkbox' name='import_comments' checked> Import Comments ";
         echo "(".count($this->data_collection->entries['comments']).")";
      }
      elseif(isset($this->data_collection->entries['comments']) && count($this->data_collection->entries['comments']) > 0)
      {
         echo "<li><input type='checkbox' name='import_comments'> Import Comments <i>requires stories</i> ";
         echo "(".count($this->data_collection->entries['comments']).")";
      }
      else
      {
         echo "<li><i>No Comments</i>";
      }
      if(isset($this->choices['attachments']) && $this->choices['attachments'] === true)
      {
         echo "<li><input type='checkbox' name='import_attachments' checked> Import Attachments ";
         echo "(".count($this->data_collection->entries['attachments']).")";
      }
      elseif(isset($this->data_collection->entries['attachments']) && count($this->data_collection->entries['attachments']) > 0)
      {
         echo "<li><input type='checkbox' name='import_attachments'> Import Attachments ";
         echo "(".count($this->data_collection->entries['attachments']).")";
      }
      else
      {
         echo "<li><i>No Attachments</i>";
      }
      if(isset($this->choices['redirects']) && $this->choices['redirects'] === true)
      {
         echo "<li><input type='checkbox' name='import_redirects' checked> Import Friendly URLs ";
      }
      elseif(isset($this->data_collection->entries['redirects']))
      {
         echo "<li><input type='checkbox' name='import_redirects'> Import Friendly URLs ";
      }

      echo "</ul></td></tr>";
      writeMenuFooter($cols, false);
   }


   function writeImportButtons($cols, &$s_o, $target)
   {
      //get strings
      $import_text = $s_o->getString("import_collection_text");
      $import_choose_text = $s_o->getString("choose_import_collection_text");
      $or_text = $s_o->getString("or");
      ?>
         <tr class='admin'>
         <td colspan='<?=$cols?>' style='text-align: center'>
            <form action='?' method='GET' style='display: inline; margin: 0;'>
            <input type='hidden' name='action' value='import'>
            <input type='hidden' name='target_collection' value='<?=$target?>'>
            <input type='submit' name='import' value='<?=$import_text?>'>
            </form> <?=$or_text?>
            <form action='?' method='GET' style='display: inline; margin: 0;'>
            <input type='hidden' name='target_collection' value='<?=$target?>'>
            <input type='hidden' name='action' value='import-choose'>
            <input type='submit' name='import' value='<?=$import_choose_text?>'>
            </form>
        </td>
        </tr>
      <?
   }

   function writeInstallButtons($cols, &$s_o, $target, $is_confirm)
   {
      //get strings
      $install_text = $s_o->getString("import_collection_text");
      ?>
         <tr class='admin'>
         <td colspan='<?=$cols?>' style='text-align: center'>
            <form action='?' method='GET' style='display: inline; margin: 0;'>
            <input type='hidden' name="file_stage_done" value="confirmed">
            <input type='hidden' name="database_stage_done" value="confirmed">
            <input type='hidden' name='collection_choice' value='<?=$target?>'>
      <?
      if($is_confirm)
      {
      ?>
            <input type='hidden' name="dataobjects_choices_done" value="confirmed">
            <input type='submit' name="dataobjects_stage_done" value="Install &gt;&gt;">
      <?
      }
      else
      {
      ?>
            <input type='submit' name="dataobjects_choices_done" value="<?=$install_text?>">
      <?
      }
      ?>
            </form>
        </td>
        </tr>
      <?
   }



   function writeMetaDataSection($cols)
   {
      writeMenuSectionHeader("Collection Basic Information", "The basic information about this data collection", $cols);
      echo "<tr class='admin'><td class='admin' style='vertical-align: middle'>ID</td><td class='admin'>";
      echo htmlspecialchars($this->data_collection->info->name, ENT_QUOTES);
      echo "</td></tr>";
      echo "<tr class='admin'><td class='admin' style='vertical-align: middle'>Title</td><td class='admin'>";
      echo htmlspecialchars($this->data_collection->info->getMeta("title"), ENT_QUOTES);
      echo "</td>";
      echo "</tr>";
      echo "<tr class='admin'><td class='admin' style='vertical-align: middle'>Description</td><td class='admin'>";
      echo htmlspecialchars($this->data_collection->info->getMeta("description"), ENT_QUOTES);
      echo "</td>";
      echo "</tr>";
      writeMenuFooter($cols, false);
   }



   function import($action, $recache = false)
   {
      $this->initialliseImportMap();
      if($action == 'import') $is_basic = true;
      else $is_basic = false;
      $this->importCategories($is_basic);
      $this->importRoles($is_basic);
      $this->importContent($is_basic);
      $this->importObjects($is_basic);
      $this->importSundries($is_basic);
      if($recache)
      {
         $this->obj_set->commitChanges(true, true);
      }
      return true;
   }

   function importCategories($is_basic = true)
   {
      global $languageList, $topicList, $typeList, $regionList, $system_config;
      $c = 'language';
      // For Oscailt 3.4 to Oscailt 3.5 types etc did not have language ids before so we
      // latch onto the min id to give a default.
      $lang_min = 99999;
      foreach($this->data_collection->entries[$c]->languages as $item)
      {
         $l = $languageList->getLanguageByCode($item->language_code);
         $old_id = $item->language_id;
         if(($l === null && ($is_basic or in_array($old_id, $this->choices["new_".$c]))) or (in_array($l->language_id, $this->choices[$c])))
         {
            if($l === null)
            {
               $item->language_id = 0;              
               $msg = "importing new language ";
            }
            else
            {
               $msg = "updating language ";
               $item->language_id = $l->language_id;
               if ($item->language_id <= $lang_min) $lang_min = $item->language_id;
            }
            $item->save();
            $languageList->load();
            $l = $languageList->getLanguageByCode($item->language_code);
            $this->import_source_map[$c][$old_id] = $l->language_id;
            $this->import_record[] = $msg . $l->language_name;
         }
         else
         {
         }
      }
      unset($this->data_collection->entries[$c]);

      if ($lang_min == 0 OR $lang_min == 99999 ) $lang_min = 1; 

      $c = 'type';
      foreach($this->data_collection->entries[$c]->types as $item)
      {
         if ($item->language_id != null && $item->language_id != 0) 
             $l = $typeList->getTypeByName($item->type_name, $item->language_id);
	 else
             $l = $typeList->getTypeByName($item->type_name);

         $old_id = $item->type_id;
         if(($l === null && ($is_basic or in_array($old_id, $this->choices["new_".$c]))) or (in_array($l->type_id, $this->choices[$c])))
         {
            if($l === null)
            {
               $msg = "importing type ";
	       // Keep this now for multi-language support
               $item->type_id = 0;
	       if ($l->language_id == null || $l->language_id == 0 || $item->language_id == null || $item->language_id == 0) $item->language_id = $lang_min;
            }
            else
            {
               $msg = "updating type ";
	       if ($l->language_id == null || $l->language_id == 0 || $item->language_id == null || $item->language_id == 0) $item->language_id = $lang_min;
               $item->type_id = $l->type_id;
            }
            $item->save();
            $typeList->load();
            $l = $typeList->getTypeByName($item->type_name);
            $this->import_source_map[$c][$old_id] = $l->type_id;
            $this->import_record[] = $msg . $l->type_name;
         }
      }
      unset($this->data_collection->entries[$c]);
      $c = 'region';
      foreach($this->data_collection->entries[$c]->regions as $item)
      {
         if ($item->language_id != null && $item->language_id != 0) 
             $l = $regionList->getRegionByName($item->region_name, $item->language_id);
	 else 
             $l = $regionList->getRegionByName($item->region_name);

         $old_id = $item->region_id;
         if(($l === null && ($is_basic or in_array($old_id, $this->choices["new_".$c]))) or (in_array($l->region_id, $this->choices[$c])))
         {
            if($l === null)
            {
               $item->region_id = 0;
	       if ($l->language_id == null || $l->language_id == 0 || $item->language_id == null || $item->language_id == 0) $item->language_id = $lang_min;
	       
               $msg = "importing region ";
            }
            else
            {
               $msg = "updating region ";
	       if ($l->language_id == null || $l->language_id == 0 || $item->language_id == null || $item->language_id == 0) $item->language_id = $lang_min;
               $item->region->id = $l->region_id;
            }
            $item->save();
            $regionList->load();
            $l = $regionList->getRegionByName($item->region_name);
            $this->import_source_map[$c][$old_id] = $l->region_id;
            $this->import_record[] = $msg . $l->region_name;
         }
      }
      unset($this->data_collection->entries[$c]);
      $c = 'topic';
      foreach($this->data_collection->entries[$c]->topics as $item)
      {
         if ($item->language_id != null && $item->language_id != 0) {
             $l = $topicList->getTopicByName($item->topic_name, $item->language_id); 
         } else {
             $l = $topicList->getTopicByName($item->topic_name);
         }

         $old_id = $item->topic_id;
         if(($l === null && ($is_basic or in_array($old_id, $this->choices["new_".$c]))) or (in_array($l->topic_id, $this->choices[$c])))
         {
            // If not found in the current installed list
            if($l === null)
            {
               $item->topic_id = 0;
	       if ($l->language_id == null || $l->language_id == 0 || $item->language_id == null || $item->language_id == 0) $item->language_id = $lang_min;
               $msg = "importing topic ";
            }
            else
            {
               $item->topic_id = $l->topic_id;
	       if ($l->language_id == null || $l->language_id == 0 || $item->language_id == null || $item->language_id == 0) $item->language_id = $lang_min;
               $msg = "updating topic ";
            }
            $item->save();
            $topicList->load();
            $l = $topicList->getTopicByName($item->topic_name);
            $this->import_source_map[$c][$old_id] = $l->topic_id;
            $this->import_record[] = $msg . $l->topic_name;
         }
      }
      unset($this->data_collection->entries[$c]);
   }

   function importRoles($is_basic)
   {
      $c = 'role';
      global $roleList;
      foreach($this->data_collection->entries[$c]->roles as $item)
      {
         $l = $roleList->getRoleByName($item->role_name);
         $old_id = $item->role_id;
         $permissions = $item->permissions;
         if(($l === null && ($is_basic or in_array($old_id, $this->choices["new_".$c]))) or (in_array($l->role_id, $this->choices[$c])))
         {
            if($l === null)
            {
               $item->role_id = 0;
               $msg = "importing role ";
            }
            else
            {
               $item->role_id = $l->role_id;
               $msg = "updating role ";
            }
            $item->permissions = null;
            $item->save();
            $this->import_record[] = $msg . $item->role_name;
            $roleList->load();
            $l = $roleList->getRoleByName($item->role_name);
            foreach($permissions as $k => $v)
            {
                $permissions[$k]->role_id = $l->role_id;
                $permissions[$k]->permission_id = null;
            }
            $l->permissions = $permissions;
            $l->savePermissions();
         }
      }
      unset($this->data_collection->entries[$c]);
   }

   function importSundries($is_basic)
   {
      global $redirectList;
      //op_r($redirectList);
      $redirectList->load();
      //op_r($redirectList);
      if($is_basic or isset($this->choices['redirects']) && $this->choices['redirects'] && isset($this->data_collection->entries['redirects']))
      {
         $new_reds = $this->data_collection->entries['redirects'];
        // op_r($new_reds);
         foreach($new_reds->redirects as $id => $red_obj)
         {
            if(in_array($id, array_keys($this->import_source_map['obj'])))
            {
               $red_obj->id = $this->import_source_map['obj'][$id];
               $redirectList->add($red_obj);
            }
         }
         $redirectList->save();
      }
      unset($this->data_collection->entries["redirects"]);
   }

   function applyInsertTransform(&$obj)
   {
      $t = $this->obj_set->supportedTypes->getType($obj->type);
      $rel_fields = $t->getCategoryRefFields();
      foreach($rel_fields as $k => $v)
      {
         $ids = $obj->getMeta($k);
         if(is_array($ids))
         {
            $new_ids = array();
            foreach($ids as $id)
            {
               if(isset($this->import_source_map[$v][$id]))
               {
                  $new_ids[] = $this->import_source_map[$v][$id];
               }
               else
               {
                  $new_ids[] = $id;
               }
            }
            $obj->setMetaData($k, $new_ids);
         }
         else
         {
            if(isset($this->import_source_map[$v][$ids]))
            {
                $obj->setMetaData($k, $this->import_source_map[$v][$ids]);
            }
         }
      }
   }

   function importObject($import_stub, $force)
   {
      global $system_config, $languageList;
      $insert_obj = $this->data_collection->obj_set->fetchObject($import_stub->obj_id, $import_stub->primary_language_code);
      $orig_id = $import_stub->obj_id;
      $master_obj = $this->data_collection->obj_set->fetchObject($import_stub->obj_id, 'master');
      if(!$master_obj or !$insert_obj)
      {
         $this->setUserMessage ( array( INDY_WARNING, "Failed to load Object files for import of Managed Object $obj_id : Import Continuing","x"));
         echo "<P>$obj_id failed to load</P>";
         return false;
      }
      //need to do the transformations on these.
      if(!$force)
      {
         $import_stub->obj_id = -1;
      }
      $import_stub->storage = $system_config->object_index_storage;
      $insert_id = $this->obj_set->forceUpdateObject($import_stub, $insert_obj, $master_obj);
      if($insert_id === false)
      {
         $this->setUserMessage ( array( INDY_WARNING, "Failed to Force Object ID Import of Managed Object $obj_id ($lang): Import Continuing","x"));
         return false;
      }
      $langs = $this->data_collection->obj_set->getObjectAvailableLanguages($import_stub->obj_id);
      array_unshift($langs, 'master');
      foreach($langs as $lang)
      {
         if($languageList->getLanguageByCode($lang))
         {
            $indy_obj = $this->data_collection->obj_set->fetchObject($orig_id, $lang);
            if(!$indy_obj)
            {
               $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Import Copy of Managed Object $obj_id ($lang): Import Aborted","42z"    ) );
               return false;
            }
            $indy_obj->objref=$insert_id;
            $this->obj_set->registerObjectUpdate($indy_obj);
         }
      }
      $this->data_collection->obj_set->unload($orig_id);
      return $insert_id;
   }

   function importObjects($is_basic)
   {
      global $system_config, $OSCAILT_SCRIPT, $languageList;
      //op_r($this->choices);
      //op_r($this->data_collection->obj_set);
      $sites = $this->data_collection->obj_set->getObjectStubsByTypename("site");
      foreach($sites as $site_stub)
      {
         $orig_site_id = $site_stub->obj_id;
         $this->import_source_map['files']["site_section_".$orig_site_id] = array();
         $create_new_site = false;
         if($this->is_new_install) $force = true;
         else $force = false;
         $css = false;
         $files = false;
         if(in_array($orig_site_id, $this->choices['obj']))
         {
            //we import the section....
            $msg = "Importing Objects from Site $orig_site_id";
            if(isset($this->choices['section_import_map'][$orig_site_id]) && $this->choices['section_import_map'][$orig_site_id] > 0)
            {
               $site_id = $this->choices['section_import_map'][$orig_site_id];
               $create_new_site = 0;
            }
            else
            {
               $create_new_site = 1;
               $site_id = 1;
            }
            if($is_basic or isset($this->choices['import_css'][$orig_site_id]) && $this->choices['import_css'][$orig_site_id] > 0)
            {
               $css = true;
            }
            if($is_basic or isset($this->choices['import_files'][$orig_site_id]) && $this->choices['import_css'][$orig_site_id] == 'all')
            {
               $files = 2;
            }
            elseif(isset($this->choices['import_files'][$orig_site_id]))
            {
               $files = 1;
            }
            //if(!isset($this->obj_set->itemCollection[$site_id]))
            if($site_id == 1 and $create_new_site)
            {
               $import_stub = $this->data_collection->obj_set->itemCollection[$orig_site_id]->copy();
               $l = $languageList->getLanguageByCode($import_stub->primary_language_code);
               if($l === null)
               {
                  $langs = $this->data_collection->obj_set->getObjectAvailableLanguages($orig_site_id);
                  foreach($langs as $lang)
                  {
                     $l2 = $languageList->getLanguageByCode($lang->getLanguageCodePrefix());
                     if($l2 !== null)
                     {
                          $l = $l2;
                          break;
                     }
                  }
               }
               if($l === null)
               {
                  $import_stub->primary_language_code = $system_config->default_language_code;
               }
               else
               {
                  $import_stub->primary_language_code = $l->getLanguageCodePrefix();
               }
               $import_stub->site_id = $site_id;
               $import_id = $this->importObject($import_stub, $force);
               $msg = "Imported Site Section $orig_site_id with ID: $import_id";

               if($import_id)
               {
                  $site_id = $import_id;
               }
               if($site_id != 1 or $this->is_new_install)
               {
                  $this->setUpSectionStorage($site_id);
               }
            }
            //that should be it for the site -> now we do the objects
            $stubs = $this->data_collection->obj_set->getObjectsBySite($orig_site_id);
            $cnt = 0;
            foreach($stubs as $s)
            {
               if(!in_array($s->obj_id, $this->choices['obj'])) continue;
               if(strtolower($s->obj_type) == 'site') continue;
               $force = false;
               if(!isset($this->choices['import_renumber'][$orig_site_id]) and isset($this->choices['obj_import_map'][$s->obj_id]) and $this->choices['obj_import_map'][$s->obj_id])
               {
                  $force = true;
               }
               elseif($is_basic && $this->basic_choices['renumber'] == false)
               {
                  $force = true;
               }
               $import_stub = $s->copy();
               $import_stub->site_id = $site_id;
               $l = $languageList->getLanguageByCode($import_stub->primary_language_code);
               if($l === null)
               {
                  $langs = $this->data_collection->obj_set->getObjectAvailableLanguages($orig_site_id);
                  foreach($langs as $lang)
                  {
                     $l2 = $languageList->getLanguageByCode($lang->getLanguageCodePrefix());
                     if($l2 !== null)
                     {
                          $l = $l2;
                          break;
                     }
                  }
               }
               if($l === null)
               {
                  continue; //object does not exist in an installed language
               }
               $insert_id = $this->importObject($import_stub, $force);
               if($insert_id)
               {
                  $cnt++;
                  $this->import_source_map['obj'][$s->obj_id] = $insert_id;//old -> new
               }
               $this->obj_set->commitChanges(false);
            }
         }
         $this->import_record[] = $msg;
         $this->import_record[] = "$cnt objects imported into section $site_id";
         //here's where we do the files....
         if($files == 2 or $files == 1)
         {
            $orig_store = $this->root_dir.$this->import_id."/site_section_".$orig_site_id."/sundries/";
            $target_store = $system_config->site_file_store_base.'site_'.$site_id.'/';
            $default_store = $system_config->site_file_store_base.'default/';
            $dh=opendir($orig_store);
            if(!$dh)
            {
               reportError("Couldn't open import directory: ".$orig_store);
            }
            while($file=readdir($dh))
            {
               if($file == 'common.css' or $file =='style.css' or $file=='contentstyle.css')
               {
                  if($css)
                  {
                     if(file_exists($default_store."/".$file))
                     {
                        if(copy($orig_store."/".$file, $target_store.$file))
                        {
                           $this->import_source_map['files']["site_section_".$orig_site_id]["sundries/".$file] = $default_store.$file;
                        }
                     }
                     else
                     {
                        if(copy($orig_store."/".$file, $default_store.$file))
                        {
                           $this->import_source_map['files']["site_section_".$orig_site_id]["sundries/".$file] = $default_store.$file;
                        }
                     }
                  }
               }
               if(!is_dir($orig_store."/".$file) and (!($file == 'common.css' or $file =='style.css' or $file=='contentstyle.css')))
               {
                  if(copy($orig_store."/".$file, $target_store.$file))
                  {
                     $this->import_source_map['files']["site_section_".$orig_site_id]["sundries/".$file] = $target_store.$file;
                  }
               }
            }
            closedir($dh);
         }
         //op_r($this->import_source_map['files']);
         //finally tidy up the inserted objects to ensure that their links and so on remain consistent.
         foreach($this->import_source_map['obj'] as $orig_id => $insert_id)
         {
            $obj_stub = $this->obj_set->itemCollection[$insert_id];
            $langs = $this->obj_set->getObjectAvailableLanguages($insert_id);
            array_unshift($langs, 'master');
            foreach($langs as $lang)
            {
               $obj = $this->obj_set->fetchObject($insert_id, $lang);
               if($obj)
               {
                  $this->applyInsertTransform($obj);
                  $obj->remapObjectReferences($this->obj_set, $this->import_source_map['obj']);
                  $obj->remapLinkedObjectReferences($this->obj_set, $this->import_source_map['files']["site_section_".$orig_site_id], $this->root_dir.$this->import_id."/site_section_".$orig_site_id."/");
                  $this->obj_set->registerObjectUpdate($obj, true);
               }
            }
            $this->obj_set->commitChanges(false);
            $this->obj_set->unload($insert_id);
         }
      }
      return true;
   }

   function setUpSectionStorage($k)
   {
      global $system_config;
      $st_store = $system_config->site_file_store_base.'site_'.$k.'/';
      if(!mkdir($st_store, $system_config->default_writable_directory_permissions))
      {
         $this->setUserMessage( array ( USER_WARNING, "Failed to Make Directory $st_store to store files associated with the site: ".$k, "sr5"));
         if(!file_exists($st_store)) return false;
      }
      if($system_config->debug_level >= DEBUG_LEVEL_1)
      {
         $this->setUserMessage(array(
            DEBUG_LEVEL_1, "Created Directory $st_store for site object ".$k." file store",   "a3y"));
      }
      return true;
   }

   function importContent()
   {
      $c_types = array("stories", "comments", "attachments");
      $c = 'stories';
      if(!isset($this->choices['stories']) or $this->choices['stories'] != true) return;
      foreach($this->data_collection->entries[$c] as $s_path)
      {
         if(file_exists($s_path)){
         	$s = unserialize(implode('',file($s_path)));
         	$old_id = $s->story_id;
         	$s->story_id = null;
         	$s->save();
         	$this->import_source_map[$c][$old_id] = $s->story_id;
         }
      }
      $this->import_record[] = "Imported ".count($this->data_collection->entries[$c]) ." ".$c;
      $c = 'comments';
      if(isset($this->choices['comments']) and $this->choices['comments'] == true)
      {
         foreach($this->data_collection->entries[$c] as $com_path)
         {
            if(file_exists($com_path))
            {
		    $com = unserialize(implode('',file($com_path)));
		    $old_id = $com->comment_id;
		    $com->comment_id = null;
		    if(isset($this->import_source_map['stories'][$com->story_id]))
		    {
		       $com->story_id = $this->import_source_map['stories'][$com->story_id];
		       $com->save();
		       $this->import_source_map[$c][$old_id] = $com->comment_id;
		    }
            }
         }
         $this->import_record[] = "Imported ".count($this->data_collection->entries[$c]) ." ".$c;
      }
      if(!isset($this->choices['attachments']) or $this->choices['attachments'] != true) return;
      $c = 'attachments';
      $dir_size = 1000;
      $cnt = 0;
      //op_r($this->import_source_map);
      foreach($this->data_collection->entries[$c] as $a_path)
      {
         $a = unserialize(implode('',file($a_path)));
         $old_id = $a->attachment_id;
         $old_file = $a->attachment_file;
         $a->attachment_id = null;
         if($a->story_id != null && isset($this->import_source_map['stories'][$a->story_id]))
         {
            $a->story_id = $this->import_source_map['stories'][$a->story_id];
            if($a->comment_id != null && isset($this->import_source_map['comments'][$a->comment_id]))
            {
               if(!isset($this->choices['comments']) or $this->choices['comments'] != true)
               {
                  continue;
               }
               $a->comment_id = $this->import_source_map['comments'][$a->comment_id];
            }
            $cnt++;
            $offset = ((int)($cnt / $dir_size)) +1;
            $this->importAttachmentFile($a, $offset);
            $a->save();
            $this->import_source_map[$c][$old_id] = $a->attachment_id;
            $this->import_source_map[$c."_files"][$old_file] = $a->attachment_file;
         }
      }
      $this->import_record[] = "Imported $cnt $c";
   }

   function importAttachmentFile(&$a, $offset)
   {
      //the directory
      global $system_config;
      $import_path = $this->root_dir.$this->import_id."/".$a->attachment_file;
      $target_dir = "import_".$this->import_id;
      $full_dir = $target_dir."/".$offset;
      if(!is_dir($system_config->attachment_store.$target_dir) && !mkdir($system_config->attachment_store.$target_dir, $system_config->default_writable_directory_permissions))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $import_dir to import $import_path file", "2sf5"));
         return false;
      }
      if(!is_dir($system_config->attachment_store.$full_dir) && !mkdir($system_config->attachment_store.$full_dir, $system_config->default_writable_directory_permissions))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $full_dir to import $import_path file", "2sf5x"));
         return false;
      }
      if(file_exists($import_path))
      {
         $p = pathinfo($import_path);
         $fname = $full_dir."/".$p['basename'];
         copy($import_path, $system_config->attachment_store.$fname);
         $a->attachment_file = $fname;
      }
      else
      {
         $this->setUserMessage( array ( INDY_ERROR, "File $import_path does not exist!", "2sf5x"));
         return false;
      }
   }


   function writeHTMLDetails()
   {
      global $system_config;
      ?>
      <table>
         <tr><th colspan="2">Category Information</th></tr>
         <tr><td>Languages</td><td>
      <?
      foreach($this->import_entries['language']->languages as $l)
      {
         echo $l->language_name." (".$l->language_code.") ";
      }
      ?>
         </td></tr>
         <tr><td>Types</td><td>
      <?
      foreach($this->import_entries['type']->types as $l)
      {
         echo $l->type_name.", ";
      }
      ?>
         </td></tr>
         <tr><td>Topics</td><td>
      <?
      foreach($this->import_entries['topic']->topics as $l)
      {
         echo $l->topic_name.", ";
      }
      ?>
         </td></tr>
         <tr><td>Regions</td><td>
      <?
      foreach($this->import_entries['region']->regions as $l)
      {
         echo $l->region_name.", ";
      }
      ?>
         <tr><th colspan="2">Role Information</th></tr>
      <?
      foreach($this->import_entries['role']->roles as $l)
      {
         echo "<tr><td>".$l->role_name."</td>";
         echo "<td>Type: $l->role_type | Permissions: ";
         foreach($l->permissions as $p)
         {
            echo $p->page;
            if($p->readonly) echo "(r)";
            else echo "(w)";
            echo ", ";
         }
      }
      ?>
          </td></tr>
          <tr><th colspan="2">Content Information</th></tr>
      <?
      $cats = array("stories", "comments", "attachments");
      foreach($cats as $c)
      {
         echo "<tr><td>$c</td><td>";
         echo count($this->import_entries[$c]);
         echo "</td></tr>";
      }
      ?>
          </td></tr>
          <tr><th colspan="2">Modules</th></tr>
      <?
      foreach($this->source_set->supportedTypes->legalTypes as $t)
      {
         $ret = $this->source_set->getObjectInfoByTypename($t, $system_config->default_language_code);
         if(count($ret) > 0)
         {
            echo "<tr><td>$t modules</td><td>";
            foreach($ret as $k => $v)
            {
               echo $v." (id: $k) ";
            }
            echo "</td></tr>";
         }
      }
      ?>
      </table>
      <?
   }

} 
?>
