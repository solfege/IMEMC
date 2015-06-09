<?php
require_once "objects/indyobjects/indyimportexport.inc";


class indyDataCollection extends indyObject
{
   var $obj_set;
   var $entries;
   var $info;
   var $id;
   var $title;
   var $description;
   var $data_loaded = false;
   var $collection_base;
   var $collection_info_path;

   var $installed_elements;
   var $new_elements;
   var $new_element_choices = array();

   function indyDataCollection($info, $base, $id = null)
   {
      $this->collection_info_path = $info;
      $this->collection_base = $base;
      $this->id = $id;
   }

   function clearChoices()
   {
   }

   function resetEntries()
   {
       $this->entries = array(
            "language" => null,
            "region" => null,
            "topic" => null,
            "type" => null,
            "role" => null,
            "stories" => null,
            "attachments" => null,
            "comments" => null,
            "css" => null,
            "links" => null,
            "redirects" => null,
            "files" => null);
       $this->installed_elements = array(
            "language" => array(),
            "region" => array(),
            "topic" => array(),
            "type" => array(),
            "role" => array(),
            "obj" => array(),
            "stories" => array(),
            "target" => array(),
            "css" => null,
            "links" => null,
            "files" => null,
            "redirects" => null,
            "update urls" => null);
       $this->new_elements = array(
            "language" => array(),
            "region" => array(),
            "topic" => array(),
            "type" => array(),
            "role" => array(),
            "obj" => array(),
            "stories" => array(),
            "target" => array(),
            "css" => null,
            "links" => null,
            "files" => null,
            "redirects" => null,
            "update urls" => null);
   }

   function load($content = false)
   {
      global $system_config, $userLanguage;
      $this->resetEntries();
      $this->clearChoices();
      $this->info = new indyItemSet();
      $this->info->load($this->collection_info_path, $this->id);
      $this->description = $this->info->getMeta('description');
      $this->title = $this->info->getMeta('title');
      $this->obj_set = new indyObjectSet($system_config->xmltypedef_dir,$this->collection_base.$this->id.".index");
      $x = new indyObjectActionRequest();
      if(!$this->obj_set->load(array("*"), array("*"), $x))
      {
         //reportError("Failed to load the object set from " .$this->collection_base.$this->id);
      }
      $this->loadCategories();
      $this->loadRoles();
      $this->loadRedirects();
      $sites = $this->obj_set->getObjectInfoByTypename("site", $userLanguage->getLanguageCodePrefix());
      foreach($sites as $k => $v)
      {
         $this->loadSundries($k);
      }
      $this->loadContent($content);
   }


   function & getAllFilePaths($dir, $check_sub_directories = false, $extension = "")
   {
      if($dir{strlen($dir)-1} != "/") $dir .= "/";
      $paths = array();
      if(!file_exists($dir) or !is_dir($dir))
      {
         echo "<P>$dir does not exist or is not a directory</P>";
         return $paths;
      }
      $dh=opendir($dir);
      if(!$dh)
      {
        reportError("Couldn't open file directory: ".$dir);
      }
      while($file=readdir($dh))
      {
         if($file == "." or $file == "..") continue;
         $fullfile = $dir.$file;
         if(is_dir($fullfile) && $check_sub_directories)
         {
            $baby_paths = $this->getAllFilePaths($fullfile, true, $extension);
            foreach($baby_paths as $b)
            {
               $paths[] = $b;
            }
         }
         $p = pathinfo($fullfile);
         if(isset($p['extension']) && ($extension == "" or $p['extension'] == $extension))
         {
            $paths[] = $fullfile;
         }
      }
      closedir($dh);
      return $paths;
   }

   function compareToInstallation(&$c_list, &$ins_objs)
   {
      global $languageList, $topicList, $typeList, $roleList, $regionList, $userLanguage;

      $c = "language";
      $this->installed_elements[$c] = array();
      $this->new_elements[$c] = array();
      $cs = $this->entries[$c]->languages;
      foreach($cs as $choice)
      {
         $r = $languageList->getLanguageByCode($choice->language_code);
         if($r !== null)
         {
            $c_list[$c][] = $r->language_id;
            $this->installed_elements[$c][] = $r->language_id;
         }
         else
         {
            $this->new_elements[$c][] = $choice->language_id;
            $c_list["new_".$c][] = $choice->language_id;
         }
      }
      $c = "topic";
      $c_list[$c] = array();
      $cs = $this->entries[$c]->topics;
      foreach($cs as $choice)
      {
         $r = $topicList->getTopicByName($choice->topic_name);
         if($r !== null)
         {
            $c_list[$c][] = $r->topic_id;
            $this->installed_elements[$c][] = $r->topic_id;
         }
         else
         {
            $this->new_elements[$c][] = $choice->topic_id;
            $c_list["new_".$c][] = $choice->topic_id;
         }
      }
      $c = "region";
      $c_list[$c] = array();
      $cs = $this->entries[$c]->regions;
      foreach($cs as $choice)
      {
         $r = $regionList->getRegionByName($choice->region_name);
         if($r !== null)
         {
            $c_list[$c][] = $r->region_id;
            $this->installed_elements[$c][] = $r->region_id;
         }
         else
         {
            $this->new_elements[$c][] = $choice->region_id;
            $c_list["new_".$c][] = $choice->region_id;
         }
      }
      $c = "type";
      $c_list[$c] = array();
      $cs = $this->entries[$c]->types;
      foreach($cs as $choice)
      {
         $r = $typeList->getTypeByName($choice->type_name);
         if($r !== null)
         {
            $c_list[$c][] = $r->type_id;
            $this->installed_elements[$c][] = $r->type_id;
         }
         else
         {
            $this->new_elements[$c][] = $choice->type_id;
            $c_list["new_".$c][] = $choice->type_id;
         }
      }
      $c = "role";
      $c_list[$c] = array();
      $cs = $this->entries[$c]->roles;
      foreach($cs as $choice)
      {
         $r = $roleList->getRoleByName($choice->role_name);
         if($r !== null)
         {
            $c_list[$c][] = $r->role_id;
            $this->installed_elements[$c][] = $r->role_id;
         }
         else
         {
            $this->new_elements[$c][] = $choice->role_id;
            $c_list["new_".$c][] = $choice->role_id;
         }
      }
      $c = "obj";
      $collection_ids = $this->obj_set->getAllObjectIds();
      $c_list[$c] = $this->obj_set->getAllObjectIds();
      if(isset($this->entries['redirects']))
      {
         $c_list['redirects'] = true;
      }
      if(isset($this->entries['css'][0]))
      {
         $c_list['css'] = true;
      }
      if(isset($this->entries['files']))
      {
         if(isset($this->entries['files']['attachments']))
         {
            $c_list['attachment_files'] = count($this->entries['files']['attachments']);
         }
         $sites = $this->obj_set->getObjectInfoByTypename("site", $userLanguage->getLanguageCodePrefix());
         foreach($sites as $si => $sn)
         {
            if(isset($this->entries['files'][$si]))
            {
               $c_list['files_'.$si] = count($this->entries['files'][$si]);
            }
         }
      }
      if(isset($this->entries['stories']))
      {
         $c_list['stories'] = true;
      }
      if(isset($this->entries['attachments']))
      {
         $c_list['attachments'] = true;
      }
      if(isset($this->entries['comments']))
      {
         $c_list['comments'] = true;
      }
   }

   function loadCategories()
   {
      global $system_config;
      $cat = "language";
      $this->entries[$cat] = new LanguageList();
      $mapped = array();
      $re =& $this->loadCategoryData($cat);
      foreach($re as $r)
      {
         $mapped[$r->language_id] = $r;
      }
      $this->entries[$cat]->languages = $mapped;
      $cat = "region";
      $this->entries[$cat] = new RegionList();
      $mapped = array();
      $re =& $this->loadCategoryData($cat);
      foreach($re as $r)
      {
         $mapped[$r->region_id] = $r;
      }
      $this->entries[$cat]->regions = $mapped;
      $cat = "type";
      $this->entries[$cat] = new TypeList();
      $mapped = array();
      $re =& $this->loadCategoryData($cat);
      foreach($re as $r)
      {
         $mapped[$r->type_id] = $r;
      }
      $this->entries[$cat]->types = $mapped;
      $cat = "topic";
      $this->entries[$cat] = new TopicList();
      $mapped = array();
      $re =& $this->loadCategoryData($cat);
      foreach($re as $r)
      {
         $mapped[$r->topic_id] = $r;
      }
      $this->entries[$cat]->topics = $mapped;
   }

   function loadRedirects()
   {
      if(file_exists($this->collection_base."redirector.inc"))
      {
         $this->entries['redirects'] = new RedirectList();
         $this->entries['redirects']->load(true, $this->collection_base."redirector.inc");
      }
   }

   function loadSundries($section)
   {
      if(file_exists($this->collection_base."site_section_".$section."/sundries/common.css"))
      {
         $this->entries['css'][]=$this->collection_base."site_section_".$section."/sundries/common.css";
      }
      if(file_exists($this->collection_base."site_section_".$section."/sundries/style.css"))
      {
         $this->entries['css'][] = $this->collection_base."site_section_".$section."/sundries/style.css";
      }
      if(file_exists($this->collection_base."site_section_".$section."/sundries/contentstyle.css"))
      {
         $this->entries['css'][]=$this->collection_base."site_section_".$section."/sundries/contentstyle.css";
      }
      $dir = $this->collection_base.'site_section_'.$section."/sundries/";
      //$this->entries['files'];
      $this->entries['files'][$section] =& $this->getAllFilePaths($dir, true);
   }


   function loadContent($read_data)
   {
      global $system_config;
      $cat = array("stories", "attachments", "comments");
      foreach($cat as $c)
      {
         if($read_data)
         {
            $this->entries[$cat] =& $this->loadCategoryData($c, true);
            $this->data_loaded = true;
         }
         else
         {
            $dir = $this->collection_base.$c;
            $this->entries[$c] =& $this->getAllFilePaths($dir, true, "dump");
         }
      }
      $dir = $this->collection_base.'attachments/files/';
      $this->entries['files']['attachments'] =& $this->getAllFilePaths($dir, true);
   }

   function loadRoles()
   {
      $cat = "role";
      $this->entries[$cat] = new RoleList();
      $this->entries[$cat]->roles =& $this->loadCategoryData($cat);
   }

   function & loadCategoryData($cat, $check_sub_directories = false)
   {
      $objs = array();
      $dir = $this->collection_base.$cat."/";
      if(!file_exists($dir) or !is_dir($dir))
      {
         return $objs;
      }

      $dh=opendir($dir);
      if(!$dh)
      {
        reportError("Couldn't open category directory: ".$dir);
      }
      while($file=readdir($dh))
      {
         if($file == "." or $file == "..") continue;
         $fullfile = $dir.$file;
         if(is_dir($fullfile) && $check_sub_directories)
         {
            $baby_objs = $this->loadCategoryData($cat."/".$file, true);
            foreach($baby_objs as $b)
            {
               $objs[] = $b;
            }
         }
         $p = pathinfo($fullfile);
         if(isset($p['extension']) && $p['extension'] == 'dump')
            $objs[] = unserialize(implode('',file($fullfile)));
      }
      closedir($dh);
      return $objs;
   }

   function writeHTMLDetails()
   {
      global $system_config;
      ?>
      <table>
         <tr><th colspan="2">Category Information</th></tr>
         <tr><td>Languages</td><td>
      <?
      foreach($this->entries['language']->languages as $l)
      {
         echo $l->language_name." (".$l->language_code.") ";
      }
      ?>
         </td></tr>
         <tr><td>Types</td><td>
      <?
      foreach($this->entries['type']->types as $l)
      {
         echo $l->type_name.", ";
      }
      ?>
         </td></tr>
         <tr><td>Topics</td><td>
      <?
      foreach($this->entries['topic']->topics as $l)
      {
         echo $l->topic_name.", ";
      }
      ?>
         </td></tr>
         <tr><td>Regions</td><td>
      <?
      foreach($this->entries['region']->regions as $l)
      {
         echo $l->region_name.", ";
      }
      ?>
         <tr><th colspan="2">Role Information</th></tr>
      <?
      foreach($this->entries['role']->roles as $l)
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
         echo count($this->entries[$c]);
         echo "</td></tr>";
      }
      ?>
          </td></tr>
          <tr><th colspan="2">Modules</th></tr>
      <?
      foreach($this->obj_set->supportedTypes->legalTypes as $t)
      {
         $ret = $this->obj_set->getObjectInfoByTypename($t, $system_config->default_language_code);
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