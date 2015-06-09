<?
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


/************************************************************************/
/* Class to represent a list of data objects
/************************************************************************/

class indyListObject extends indyBasicObject
{ 
   var $mydisplay_depth = LONG_LIST_DEPTH; 
   var $loaded_content = array(); 
   var $max_nonspecific_available_listing = 50; 
   var $display_options = array( 
      "Short Title", 
      "Icon", 
      "Icon And Title", 
      "Long Title", 
      "List, without title", 
      "Title and List", 
      "Long Title and List", 
      "Full Page" 
   ); 

   function getDisplayOptions($depth) 
   { 
      $ret = $this->display_options; 
      return $ret; 
   } 

   function getOverrideOptions(&$caller_settings) 
   { 
   }

   function indyListObject ($a, $b) 
   { 
      $this->indyBasicObject($a, $b); 
   }


   /************************************************************************/ 
   /* overrides basic function to support reading in container fields 
   /************************************************************************/ 
   function loadFromForm(&$obj_set, $update_containers=true) 
   { 
      $validated = parent::loadFromForm($obj_set); 
      $obj_stubs = $obj_set->itemCollection; 
      $t = strtolower( $this->getType() ); 
      $my_type = $obj_set->supportedTypes->getType( $t ); 
      $containers = $my_type->getListingFields();

      $this->containedItems = array();//clear the current set

      foreach($containers as $container) 
      {
         if($this->language=='master' && isset($_REQUEST['enable_master_checkboxes']))
         {
              if(!isset($_REQUEST['enable_master_'.$container]) or $_REQUEST['enable_master_'.$container] != 'on')
	      continue;
         }
         if($update_containers or isset($_REQUEST[$container.'_add_objects'])) 
         { 
            $this->loadContainerFromForm($container, true); 
         } 
         else 
         { 
            $this->loadContainerFromForm($container, false); 
         }

         if(isset($_REQUEST[$container."_add_content"])) 
         { 
            $this->addContentItemToContainer($container); 
         } 
      }

      return $validated; 
   }

   function addContentItemToContainer($container) 
   { 
      //check to ensure that id exists.  
      if(isset($_REQUEST[$container."_add_content_type"]) and isset($_REQUEST[$container."_add_content_id"])) 
      { 
         $type = $_REQUEST[$container."_add_content_type"]; 
         $id = $_REQUEST[$container."_add_content_id"]; 
         if($this->contentItemExists($type, $id)) 
         { 
            $r_s = array(); 
            $content_id = 'content_'.$type.'_'.$id; 
            $ref = new indyObjectRef($content_id, $r_s); 
            $this->containedItems[$container][] = $ref; 
         } 
      } 
   }

   function getContentItemName($type, $id) 
   { 
      if($this->contentItemExists($type, $id)) 
      { 
         if($type == 'story') 
         { 
            $n = $this->loaded_content[$type][$id]->story_title; 
         } 
         elseif($type == 'comment') 
         { 
            $n = $this->loaded_content[$type][$id]->comment_title; 
         } 
         elseif($type == 'attachment') 
         { 
            $n = $this->loaded_content[$type][$id]->description; 
         }

         if(strlen($n) > 60) $n = substr($n, 0, 60); 
         elseif($n == "") $n = "none available"; 
         return $n; 
      } 
      return "$type $id does not exist"; 
   }

   function getContentItem($type, $id) 
   { 
      if($this->contentItemExists($type, $id)) 
      { 
         return $this->loaded_content[$type][$id]; 
      } 
      return false; 
   }

   function contentItemExists($type, $id) 
   { 
      if(!isset($this->loaded_content[$type])) $this->loaded_content[$type] = array(); 
      elseif(isset($this->loaded_content[$type][$id])) return true; 

      if($type == 'story') 
      { 
         require_once('objects/story.inc'); 
         $story = new Story(); 
         $story->story_id = $id; 
         if($story->load()) 
         { 
            $this->loaded_content[$type][$id] = $story; 
            return true; 
         } 
      } 
      elseif($type == 'comment') 
      { 
         require_once('objects/comment.inc'); 
         $comment = new Comment(); 
         $comment->comment_id = $id; 
         if($comment->load()) 
         { 
            $this->loaded_content[$type][$id] = $comment; 
            return true; 
         } 
      } 
      elseif($type=='attachment') 
      { 
         require_once('objects/attachment.inc'); 
         $attachment = new Attachment(); 
         $attachment->attachment_id = $id; 
         if($attachment->load()) 
         { 
            $this->loaded_content[$type][$id] = $attachment; 
            return true; 
         } 
      } 
      return false; 
   }

   function loadContainerFromForm($container, $update_inclusions) 
   { 
      $this->containedItems[$container] = array(); 
      $objects_to_check = array(); 
      $included_objs = array();

      if(isset($_REQUEST[$container.'_included_objects'])) 
      { 
         $objects_to_check = explode(",", $_REQUEST[$container.'_included_objects']); 
      }

      if($update_inclusions) 
      { 
         if(isset($_REQUEST[$container.'_potential_objects'])) 
         { 
            $extra_objects = explode(",", $_REQUEST[$container.'_potential_objects']); 
            foreach($extra_objects as $id) 
            { 
               if(!in_array($id, $objects_to_check)) $objects_to_check[] = $id; 
            } 
         } 
      } 

      foreach($objects_to_check as $id) 
      { 
         if(!$update_inclusions or (isset($_REQUEST[$container."_include_".$id]) && $_REQUEST[$container."_include_".$id] == 'on')) 
         { 
            $included_objs[] = $this->loadObjectReferenceFromForm($container, $id); 
         } 
      }

      $this->orderIncludedItems($included_objs, $container); 
   }

   function orderIncludedItems(&$included_items, $container) 
   { 
      $ordered_items = array(); 
      $unordered_items = array();

      foreach($included_items as $o_ref) 
      { 
         $pos = ( isset( $_REQUEST[$container.'_position_'.$o_ref->id] ) ) ? (int)($_REQUEST[$container.'_position_'.$o_ref->id]) : null;

         if(($pos == null) || ($pos <= 0) || (in_array($pos, array_keys($ordered_items)))) 
         { 
            $unordered_items[] = $o_ref; 
         } 
         else 
         { 
            $ordered_items[$pos] = $o_ref; 
         } 
      }

      ksort ($ordered_items); 
      foreach($unordered_items as $item) 
      { 
         $ordered_items[] = $item; 
      }

      foreach($ordered_items as $item) 
      { 
         $this->containedItems[$container][] = $item; 
      } 
   }

   function loadObjectReferenceFromForm($container, $id) 
   { 
      $r_s = array(); 
      if(isset( $_REQUEST[$container.'_style_'.$id] )) 
      { 
         $r_s['style'] = $_REQUEST[$container.'_style_'.$id]; 
      }

      if(isset( $_REQUEST[$container.'_depth_'.$id] )) 
      { 
         $r_s['depth'] = $_REQUEST[$container.'_depth_'.$id]; 
      }

      if(isset( $_REQUEST[$container.'_hsize_'.$id] )) 
      { 
         $r_s['hsize'] = $_REQUEST[$container.'_hsize_'.$id]; 
      }

      if(isset( $_REQUEST[$container.'_surround_'.$id] )) 
      { 
         $r_s['surround'] = $_REQUEST[$container.'_surround_'.$id]; 
      }

      if(isset( $_REQUEST[$container.'_override_protected_'.$id] )) 
      { 
         $r_s['override_protected'] = $_REQUEST[$container.'_override_protected_'.$id]; 
      }

      $obj_ref = new indyObjectRef($id, $r_s);

      return $obj_ref; 
   }

   function writeHiddenObjectListing($container, &$obj_set) 
   { 
      if(isset($_REQUEST[$container.'_potential_objects'])) 
      { 
          echo "<input type='hidden' name='$container"."_potential_objects' value='".$_REQUEST[$container.'_potential_objects']."'>\n"; 
      }
      if($this->display_master_options)
      {
      	 $val = (isset($_REQUEST['enable_master_'.$container])) ? $_REQUEST['enable_master_'.$container] : "";
         echo "<input type='hidden' name='enable_master_".$container."' value='$val'>\n";
      }

      $t = strtolower( $this->getType() );

      $my_type = $obj_set->supportedTypes->getType( $t );

      if(!in_array($container, $my_type->getListingFields())) 
      { 
         echo "<p>failed to write hidden listing $container</p>"; 
         return; 
      }

      //$tInfo = $obj_set->supportedTypes->getListingInfo ( $t, $container ); 
      //$type_obj = $obj_set->supportedTypes->getType($t); 
      if(!isset($this->containedItems[$container]) or count($this->containedItems[$container]) == 0) 
      { 
         return ""; 
      } 
      else 
      { 
         $included_items = array(); 
         for($i = 0; $i<count($this->containedItems[$container]); $i++) 
         { 
            $objref = $this->containedItems[$container][$i]; 
            if(substr($objref->id, 0, 8) == 'content_') 
            { 
               $parts = explode("_", $objref->id); 
               if(!isset($parts[2])) continue; 
               $content_type = $parts[1]; 
               $content_id = $parts[2]; 
               $obj = new indyContentObject($objref->id, $content_type); 
            } 
            else 
            { 
               if(!isset($obj_set->itemCollection[$objref->id])) 
               { 
                  $this->setUserMessage ( array ( INDY_WARNING, "Object in list with ID: $nextObjRef->id, contained in container $container of object $this->objref does not exist in this collection", "xv" )); 
                  continue; 
               } 
               $next_obj_stub = $obj_set->itemCollection[$objref->id]; 
               $next_obj_type = $next_obj_stub->obj_type; 
               $contained_type_obj = $obj_set->supportedTypes->getType($next_obj_type); 
               $obj = $obj_set->fetchObject($objref->getId(), $this->getLangCode()); 
               if(!$obj) 
               { 
                  $this->setUserMessage ( array ( INDY_ERROR, "Non Existant Object in list with ID: ".$nextObjRef->getId(), "" ) ); 
                  continue; 
               } 
            }

            $included_items[] = $obj->id(); 
            $hiddens[$container."_".'include'."_".$obj->id()] = "on"; 
            $hiddens[$container."_".'position'."_".$obj->id()] = $i + 1; 
            $hiddens[$container."_".'style'."_".$obj->id()] = $objref->getStyle(); 
            $hiddens[$container."_".'depth'."_".$obj->id()] = $objref->getDepth(); 
            $hiddens[$container."_".'hsize'."_".$obj->id()] = $objref->getSetting('hsize'); 
            $hiddens[$container."_".'override_protected'."_".$obj->id()]  = $objref->getOverrideProtected(); 
            $hiddens[$container."_".'surround'."_".$obj->id()] = $objref->getBox();

            foreach(array_keys($hiddens) as $h) 
            { 
               echo $this->getHiddenEntryHTML($h, $hiddens[$h]); 
            } 
         } 
         echo "<input type='hidden' name='$container"."_included_objects' value='".implode(",", $included_items)."'>\n"; 
      } 
   }

   function writeObjectListing($container, &$o) 
   { 
      $t = strtolower( $this->getType() ); 
      $my_type = $o->supportedTypes->getType( $t );

      if(!in_array($container, $my_type->getListingFields())) 
      { 
         echo "<p>failed to write listing $container for $t</p>Valid Listing fields:"; 
         op_r($my_type->getListingFields()); 
      } 
      else 
      { 
         $tInfo = $o->supportedTypes->getListingInfo ( $t, $container, "included");

         $contents = $this->getListHTML($o, $my_type, $tInfo, $container);
         list($lockdirective, $is_locked) = $this->getMasterLockingInformation($container);
         echo $this->getListingFormFieldHTML($contents, $is_locked, "", $container);
      } 
   }


   /************************************************************************/ 
   /* function to wrap the basic form output in html 
   /* Input: title of field, description, help text, contents (all strings) 
   /*        boolean locked, 
   /* output: html string 
   /************************************************************************/ 
   function getListingFormFieldHTML($contents, $is_locked, $status_html, $name="") 
   { 
      $translate_contents = ""; 
      if($this->calling_req->getObjectLanguage() == 'master' xor $is_locked ) 
      { 
         $cls_str = 'class="edit-object-locked"'; 
         $entry_str = 'class="edit-object-locked-contents"'; 
         $cls_spacer = 'class="edit-object-locked-spacer"'; 
      } 
      else 
      { 
         $cls_str = 'class="edit-object"'; 
         $entry_str = 'class="edit-object-contents"'; 
         $cls_spacer = 'class="edit-object-spacer"'; 
      }

      if($this->display_master_options) 
      { 
         $master_html = $this->getMasterLockingHTML($is_locked, $name); 
      } 
      else 
      { 
         $master_html = $this->getMasterLockedHTML($is_locked); 
      }

      $str = "<tr $entry_str>\n"; 
      $str .= "<td $entry_str>$master_html</td>\n"; 
      $str .= "<td $cls_str colspan='2'>$contents"; 
      $str .= "</td>\n"; 
      $str .= "</tr>\n"; 
      $str .= "<tr $cls_spacer>\n"; 
      $str .= "<td $cls_spacer colspan='3'>\n"; 
      $str .= "</td></tr>\n"; 
      return $str; 
   }

   function drawObjectViewHeader(&$obj_set) 
   { 
      $my_link = $obj_set->calling_req->getLink(); 
      if( isset( $_REQUEST['view_type'] ) && $_REQUEST['view_type'] == "advanced") 
      { 
         $viewlink = "<small>( <a href='$my_link'>standard</a> / <strong>advanced</strong> )</small><input type='hidden' name='view_type' value='advanced'>"; 
      } 
      else 
      { 
         $viewlink = "<small>( <strong>standard</strong> / <a href='$my_link&view_type=advanced'>advanced</a> )</small>"; 
      } 
      $viewlink .= "<input type='hidden' name='form-loaded' value='youbet'>";

      return $viewlink; 
   }

   function drawJSDisabler() 
   { 
      $js = '<SCRIPT TYPE="text/javascript"> 
               <!-- 
               function do_disable(y, x) 
               { 
                  if(y.checked){ 
                     x.disabled = false; 
                  }else { 
                     x.disabled = true; 
                  } 
               } 
               //--> 
      </SCRIPT>'; 
      return $js; 
   } 

   function drawAsForm(&$o, $tr_mode) 
   { 
      if(isset($_REQUEST['form-loaded']) && !isset($_REQUEST['preview'])) 
      { 
         $this->loadFromForm($o, false); 
      } 
      echo ($this->drawObjectViewHeader($o)); 
      echo ($this->drawJSDisabler());

      return parent::drawAsForm($o, $tr_mode); 
   }

   function getListHTML(&$obj_set, &$type_obj, $listInfo, $container = 'contained') 
   { 
      $included_objects = array(); 
      $potential_objects = array(); 

      $cols = 1 + count( $listInfo['options'] );

      if( isset( $_REQUEST['view_type'] ) && $_REQUEST['view_type'] == "advanced") 
      { 
         $adv = true; 
         $cols += count ( $listInfo['advancedoptions'] ); 
      } 
      else 
      { 
         $adv = false; 
      } 

      $retstr = $this->getListTableHeader(); 
      $retstr .= $this->getListSectionSpacer($cols); 
      $retstr .= $this->getListSectionHeader($cols, $type_obj, $container, $listInfo['includedhelp'], true); 
      $retstr .= $this->getListColumnHeaders( $listInfo['options'], $listInfo['advancedoptions'], $adv);

      if(!isset($this->containedItems[$container]) or count($this->containedItems[$container]) == 0) 
      { 
         $retstr .= $this->getEmptyContainerRow($cols, $type_obj, $container); 
      } 
      else 
      { 
         for($i = 0; $i<count($this->containedItems[$container]); $i++) 
         { 
            $nextObjRef = $this->containedItems[$container][$i]; 
            //put in the content listing here.

            if(substr($nextObjRef->id, 0, 8) == 'content_') 
            { 
               $parts = explode('_', $nextObjRef->id); 
               if(!isset($parts[2])) continue;

               $content_type = $parts[1]; 
               $content_id = $parts[2]; 
               $retstr .= $this->getListContentEntry($i, $obj_set, $content_type, $content_id, $nextObjRef, $listInfo, $adv, $container); 
            } 
            else 
            { 
               if(!isset($obj_set->itemCollection[$nextObjRef->id])) 
               { 
                  $this->setUserMessage ( array ( INDY_WARNING, "Object in list with ID: $nextObjRef->id, contained in container $container of object $this->objref does not exist in this collection", "xv" )); 
                  continue; 
               }

               $next_obj_stub = $obj_set->itemCollection[$nextObjRef->id]; 
               $next_obj_type = $next_obj_stub->obj_type; 
               $contained_type_obj = $obj_set->supportedTypes->getTypeDetails($next_obj_type);

               if(!$contained_type_obj) 
               { 
                  $this->setUserMessage ( array ( INDY_WARNING, "Object in list with Type: $next_obj_type, contained in container $container of object $this->objref does not exist in this collection", "xv" )); 
                  continue; 
               } 
               $retstr .= $this->getListEntry($i, $obj_set, $contained_type_obj, $nextObjRef, $listInfo, $adv, $container, true); 
            } 
            $included_objects[] = $nextObjRef->id; 
         } 
      }

      $retstr .= $this->getAddSectionObjects($cols, $container); 
      $obj_stubs = $this->getDesiredAvailableObjectStubs($obj_set, $container, $listInfo);

      $retstr .= $this->getListSectionSpacer($cols); 
      $retstr .= $this->getListSectionSpacer($cols);

      $retstr .= $this->getListSectionHeader($cols, $type_obj, $container, $listInfo['availablehelp'], false); 
      $retstr .= $this->getAvailableObjectFilter($cols, $obj_set, $type_obj, $listInfo, $container);

      $retstr .= $this->getListColumnHeaders( $listInfo['options'], $listInfo['advancedoptions'], $adv); 

      $all_ids = array(); 

      if(!isset($this->containedItems[$container])) $this->containedItems[$container] = array();

      foreach($this->containedItems[$container] as $item) 
      { 
         $all_ids[] = $item->getId(); 
      }

      $cnt = 0; 
      foreach($obj_stubs as $objstub) 
      { 
         //figure out if we are interested in the object.  
         $interested = true; 
         if( in_array( $objstub->obj_id, $all_ids ) ) $interested = false; 
         if( isset($listInfo['excludes']) && in_array( strtolower( $objstub->obj_type ), $listInfo['excludes'] ) ) $interested = false; 
         elseif( isset ( $listInfo['includes'] ) && !in_array(  strtolower( $objstub->obj_type ), $listInfo['includes'] ) ) $interested = false; 
         if($objstub->obj_id == $this->objref) $interested = false;

         if ( $interested ) 
         { 
            $cnt++; 
            $obj = $obj_set->fetchObject($objstub->obj_id, $this->getLangCode());

            if(!$obj) 
            { 
               $this->setUserMessage ( array ( INDY_WARNING, "Failed to fetch $objstub->obj_id, ".$this->getLangCode(), "xv" )); 
               continue; 
            }

            if(!$obj->isActive() and ($objstub->site_id != $obj_set->itemCollection[$this->objref]->site_id)) continue; 
            elseif($obj->containsObject($this->objref, $obj_set)) continue;

            $t_obj = $obj_set->supportedTypes->getTypeDetails($obj->type); 
            if(!$t_obj) 
            { 
               $this->setUserMessage ( array ( INDY_WARNING, "Type Object in collection with ID: $obj->objref, with type $obj->type does not exist in this collection", "xv" )); 
               continue; 
            } 
            $retstr .= $this->getListEntry($cnt, $obj_set, $t_obj, $obj, $listInfo, $adv,  $container, false); 
            $potential_objects[] = $objstub->obj_id; 
         } 
      }

      if($cnt == 0) 
      { 
         $retstr .= $this->getEmptyAvailableSection($cols, $type_obj, $container); 
      } 
      $retstr .= $this->getAddSectionObjects($cols, $container); 
      $retstr .= $this->getListSectionSpacer($cols); 
      $retstr .= $this->getListSectionSpacer($cols);

      if((isset($listInfo['excludes']) && !in_array('content', $listInfo['excludes'])) or 
         (isset($listInfo['includes']) && in_array('content', $listInfo['includes']))) 
      { 
         $retstr .= $this->getContentAdditionSection($cols, $container, $listInfo['contentshelp']); 
      } 
      $retstr .= $this->getListTableFooter(); 
      $retstr .= $this->getRelevantObjectList($container, $included_objects, $potential_objects); 
      return $retstr; 
   }

   function getAddSectionObjects($cols, $container) 
   { 
      $retstr = "<tr class='object-listing'>\n<td class='object-listing-filter' colspan='$cols'>"; 
      $retstr .= "<input type='submit' name='$container".  
      "_add_objects' value='Update Contained Objects'>"; 
      $retstr .= "</td></tr>\n"; 
      return $retstr; 
   } 

   function getRelevantObjectList($container, $included_objects, $potential_objects) 
   { 
      $retstr = "\n<input type='hidden' name='$container"."_included_objects' value='"; 
      $retstr .= implode(",", $included_objects) . "'>\n"; 
      $retstr .= "\n<input type='hidden' name='$container"."_potential_objects' value='"; 
      $retstr .= implode(",", $potential_objects) . "'>\n"; 
      return $retstr;

   } 

   function getAvailableObjectFilter($cols, &$obj_set, &$type_obj, &$listInfo, $cnt) 
   { 
      global $userLanguage; 
      $this_site_id = $obj_set->itemCollection[$this->objref]->site_id; 
      if(isset($_REQUEST[$cnt."_typefilter"])) $filtered_type = $_REQUEST[$cnt."_typefilter"]; 
      else $filtered_type = "";

      if(isset($_REQUEST[$cnt."_sitefilter"])) $filtered_site = $_REQUEST[$cnt."_sitefilter"]; 
      else $filtered_site = $this_site_id;

      $str ="<tr class='object-listing'>\n<td class='object-listing-filter' colspan='$cols'>"; 
      $str .= "<select class='object-listing-filter' name='$cnt"."_typefilter'><option value=''>None</option>"; 
      $obj_set->supportedTypes->loadAllTypeDetails(); 
      $types = $obj_set->supportedTypes->getAllTypeDetails();

      foreach($types as $one_type) 
      { 
         if( isset($listInfo['excludes']) && in_array( strtolower( $one_type->type ), $listInfo['excludes'] ) ) continue; 
         elseif( isset ( $listInfo['includes'] ) && !in_array(  strtolower( $one_type->type ), $listInfo['includes'] ) ) continue; 
         elseif(strtolower($one_type->type) == 'basic') continue;

         if(strtolower($one_type->type) == strtolower($filtered_type)) 
         { 
            $str .= "<option value='$one_type->type' selected>".$one_type->name."</option>"; 
         } 
         else 
         { 
            $str .= "<option value='$one_type->type'>".$one_type->name."</option>"; 
         } 
      }

      if($filtered_type == "*") 
      { 
         $str .= "<option value='*' selected>All valid types</option>"; 
      } 
      else 
      { 
         $str .= "<option value='*'>All types</option>"; 
      } 
      $str .= "</select>"; 
      $sites = $obj_set->getObjectsByTypename ("site", $userLanguage->getLanguageCodePrefix()); 
      $this_site_id = $obj_set->itemCollection[$this->objref]->site_id; 
      $str .= " <select class='object-listing-filter' name='$cnt"."_sitefilter'>\n";

      if($filtered_site == $this_site_id) 
      { 
         $str .= "<option value='$this_site_id' selected>This section ($this_site_id)</option>\n"; 
      } 
      else 
      { 
         $str .= "<option value='$this_site_id'>This section ($this_site_id)</option>\n"; 
      }

      foreach($sites as $one_site) 
      { 
         if($one_site->objref == $this_site_id) continue; 
         if($one_site->objref == $filtered_site) 
         { 
            $str .= "<option value='$one_site->objref' selected>"; 
         } 
         else 
         { 
            $str .= "<option value='$one_site->objref'>"; 
         } 
         $str .= $one_site->name() ." ($one_site->objref)</option>\n"; 
      }

      if($filtered_site == "*") 
      { 
         $str .= "<option value='*' selected>All sections</option>\n"; 
      } 
      else 
      { 
         $str .= "<option value='*'>All sections</option>\n"; 
      } 
      $str .= "</select>\n"; 
      $str .= " <input type='submit' class='object-listing-filter' name='$cnt"."_change_available_objects' value='Show &gt; &gt;'> "; 
      $str .=   "</td>\n</tr>\n";

      return $str; 
   } 

   function isContainableType(&$listInfo, $t_name) 
   { 
      if( isset($listInfo['excludes']) && in_array( strtolower( $t_name ), $listInfo['excludes'] ) ) 
      return false; 
      elseif( isset ( $listInfo['includes'] ) && !in_array(  strtolower( $t_name ), $listInfo['includes'] ) ) return false; 
      return true; 
   }

   function getDesiredAvailableObjectStubs(&$obj_set, $container, &$listInfo) 
   { 
      $save_stubs = $obj_set->itemCollection; 
      $desired_stubs = array();

      //if(isset($_REQUEST[$container."_change_available_objects"])) 
      if(isset($_REQUEST["form-loaded"])) 
      { 
         $desired_type = $_REQUEST[$container."_typefilter"]; 
         $desired_site = $_REQUEST[$container."_sitefilter"]; 
      } 
      else 
      { 
         $all_site_obj_stubs = $obj_set->getObjectsBySite($obj_set->itemCollection[$this->objref]->site_id);

         foreach($all_site_obj_stubs as $stub) 
         { 
            if($this->isContainableType($listInfo, $stub->obj_type)) $desired_stubs[] = $stub; 
         }

         if(count($desired_stubs) > $this->max_nonspecific_available_listing) 
         { 
            $desired_stubs = array(); 
         } 
         else 
         { 
            $_REQUEST[$container."_typefilter"] = "*"; 
            $_REQUEST[$container."_sitefilter"] = $obj_set->itemCollection[$this->objref]->site_id; 
         } 
         return $desired_stubs; 
      }

      if($desired_type == "") return $desired_stubs; 
      if($desired_type != "*") 
      { 
         $obj_set->itemCollection = $obj_set->getObjectStubsByTypename($desired_type); 
      } 
      if($desired_site != "*") 
      { 
         $obj_set->itemCollection = $obj_set->getObjectsBySite($desired_site); 
      } 
      $desired_stubs = $obj_set->itemCollection; 
      $obj_set->itemCollection =& $save_stubs; 
      return $desired_stubs; 
   } 

   function getJSDisable($adv, $id, &$listInfo, $container) 
   { 
      $jscript = "JavaScript:"; 
      $options = $listInfo['options'];

      foreach($options as $option) 
      { 
         if($this->isListInputField($option)) 
         { 
            $jscript .= 'do_disable(this, document.editobjectform.'.$container.'_'.$option."_".$id.');'; 
         } 
      } 
      if($adv) 
      { 
         $advoptions = $listInfo['advancedoptions']; 
         foreach($advoptions as $option) 
         { 
            if($this->isListInputField($option)) 
            { 
               $jscript .= 'do_disable(this, document.editobjectform.'.$container.'_'.$option."_".$id.');'; 
            } 
         } 
      }

      if(strlen($jscript) > 0) 
      { 
         $jscript = 'onChange="'.$jscript.'"'; 
      } 
      return $jscript; 
   } 

   function isListInputField($opt) 
   { 
      if($opt == 'surround' or $opt == 'position' or $opt == 'override_protected' or $opt == 'style' or $opt == 'depth' or $opt=='hsize') return true; 
      return false; 
   } 

   function getListContentEntry($i, $obj_set, $content_type, $content_id, $obj_ref, $listInfo, $adv, $container) 
   { 
      $vals['position'] = $i + 1; 
      $vals['enabled'] = ''; 
      $vals['style'] = $obj_ref->getStyle(); 
      $vals['depth'] = $obj_ref->getDepth(); 
      $vals['hsize'] = $obj_ref->getSetting('hsize'); 
      $vals['override_protected'] = ( $obj_ref->getOverrideProtected()=="on" ) ? " checked" : ""; 
      $vals['surround'] = ( $obj_ref->getBox()=="on" ) ? " checked" : ""; 
      $vals['content_id'] = $content_id; 
      $vals['content_type'] = $content_type; 

      $content_obj = new indyContentObject($obj_ref->id, $content_type); 
      $type_obj = new indyDataType(); 
      $type_obj->name = $content_type; 
      $n = $container."_depth_".$content_obj->id(); 
      $sel_html = $content_obj->getDisplayOptionsSelect($this->mydisplay_depth, $vals['depth'], $n, true);

      if($sel_html) 
      { 
         $vals['depth'] = $sel_html; 
      } 
      else 
      { 
         $vals['depth'] = "not applicable"; 
      } 

      $n = $container."_hsize_".$content_obj->id(); 
      $sel_html = $content_obj->getHsizeSelect($vals['hsize'], $n, true); 
      if($sel_html) 
      { 
         $vals['hselect'] = $sel_html; 
      } 
      else 
      { 
         $vals['hselect'] = "not applicable"; 
      }


      $jscript = $this->getJSDisable($adv, $content_obj->id(), $listInfo, $container); 
      $options = $listInfo['options']; 
      $advanced_options = $listInfo['advancedoptions']; 
      $fields = array(); 
      $hiddens = array();

      foreach($options as $option) 
      { 
         $fields[] = $this->getListContentEntryField($option, $type_obj, $content_obj, $container, $jscript, $vals, $obj_set); 
      }

      foreach ( $advanced_options as $option ) 
      { 
         if( $adv ) 
         { 
            $fields[] = $this->getListContentEntryField($option, $type_obj, $content_obj, $container, $jscript, $vals, $obj_set); 
         } 
         else 
         { 
            $hiddens[] = $this->getListEntryFieldAsHidden($option, $content_obj, $container, $vals); 
         } 
      } 
      return $this->getListEntryHTML($fields, $hiddens, $i); 
   }

   function getListEntry( $i, &$obj_set, $type_obj, &$objref, &$listInfo, $adv, $container, $is_contained ) 
   { 
      $vals = array(); 
      if($is_contained) 
      { 
         //do the stuff about getting the contained values from the objref 
         $obj = $obj_set->fetchObject($objref->getId(), $this->getLangCode()); 
         if(!$obj) 
         { 
            $this->setUserMessage ( array ( INDY_ERROR, "Non Existant Object in list with ID: ".$objref->getId(), "" ) ); 
            return false; 
         }

         $vals['position'] = $i + 1; 
         $vals['enabled'] = ''; 
         $vals['style'] = $objref->getStyle(); 
         $vals['depth'] = $objref->getDepth(); 
         $vals['hsize'] = $objref->getSetting('hsize'); 
         $vals['override_protected'] = ( $objref->getOverrideProtected()=="on" ) ? " checked" : ""; 
         $vals['surround'] = ( $objref->getBox()=="on" ) ? " checked" : ""; 
      } 
      else 
      { 
         $obj = $objref; 
         $vals['position'] = 0; 
         $vals['hsize'] = 0; 
         $vals['enabled'] = " disabled"; 
         $vals['style'] = ""; 
         $vals['depth'] = ""; 
         $vals['override_protected'] = ""; 
         $vals['surround'] = ""; 
      }

      //figure out display options for the object 
      $n = $container."_depth_".$obj->id(); 
      $sel_html = $obj->getDisplayOptionsSelect($this->mydisplay_depth, $vals['depth'], $n, $is_contained); 
      if($sel_html) 
      { 
         $vals['depth'] = $sel_html; 
      } 
      else 
      { 
         $vals['depth'] = "not applicable"; 
      } 

      $n = $container."_hsize_".$obj->id(); 
      $sel_html = $obj->getHsizeSelect($vals['hsize'], $n, $is_contained); 
      if($sel_html) 
      { 
         $vals['hselect'] = $sel_html; 
      } 
      else 
      { 
         $vals['hselect'] = "not applicable"; 
      } 

      $jscript = $this->getJSDisable($adv, $obj->id(), $listInfo, $container); 
      $options = $listInfo['options']; 
      $advanced_options = $listInfo['advancedoptions']; 
      $fields = array(); 
      $hiddens = array();

      foreach($options as $option) 
      { 
         $fields[] = $this->getListEntryField($option, $type_obj, $obj, $container, $jscript, $vals, $obj_set); 
      }

      foreach ( $advanced_options as $option ) 
      { 
         if( $adv ) 
         { 
            $fields[] = $this->getListEntryField($option, $type_obj, $obj, $container, $jscript, $vals, $obj_set); 
         } 
         elseif($is_contained) 
         { 
            $hiddens[] = $this->getListEntryFieldAsHidden($option, $obj, $container, $vals); 
         } 
      }

      return $this->getListEntryHTML($fields, $hiddens, $i); 
   }

   function getListEntryHTML(&$fields, &$hiddens, $i) 
   { 
      if(($i % 2) == 0) 
      { 
         $retstr = "<tr class='object-listing'>\n"; 
      } 
      else 
      { 
         $retstr = "<tr class='object-listing2'>\n"; 
      } 
      $retstr .= implode("\n", $hiddens);

      for($i = 0; $i<count($fields); $i++) 
      { 
         if($i == 0) $cls = 'object-listing-left'; 
         elseif($i == (count($fields) - 1)) $cls = 'object-listing-right'; 
         else $cls = 'object-listing'; 
         $retstr .= "<td class='$cls'>".$fields[$i][1]."</td>\n"; 
      } 
      $retstr .= "</tr>\n"; 
      return $retstr; 
   } 

   function getListContentEntryField($option, &$type_obj, &$obj, $container, $jscript, &$vals, &$obj_set) 
   { 
      $f = array(); 
      if($option == 'name') 
      { 
         $f[0] = ''; 
         $f[1] = $this->getContentItemName($vals['content_type'], $vals['content_id']); 
      } 
      elseif($option == 'id') 
      { 
         $f[0] = "number"; 
         $f[1] = $vals['content_id']; 
      } 
      elseif( $option == "view" || $option == "edit" || $option == 'xmlview' ) 
      { 
         $f[0] = "blank"; 
         $f[1] = ""; 
      } 
      elseif($option == 'active') 
      { 
         $f[0] = 'image'; 
         $f[1] = "<img src='graphics/active.gif' alt='active' title='$type_obj->name is shared'>"; 
      } 
      elseif ( $option == "type" ) 
      { 
         $f[0] = ""; 
         $f[1] = $vals['content_type']; 
      } 
      else 
      { 
         $f = $this->getListEntryField($option, $type_obj, $obj, $container, $jscript, $vals, $obj_set); 
      }

      return $f; 
   } 

   /*function getListContentEntryFieldAsHidden($option, &$obj, $container, &$vals) 
   { 
   } 
   */ 

   function getListEntryFieldAsHidden($option, &$obj, $container, &$vals) 
   { 
      if ($this->isListInputField($option)) 
      { 
         $n = $container."_".$option."_".$obj->id(); 
         $v = $vals[$option]; 
         return $this->getHiddenEntryHTML($n, $v); 
      }

      return ""; 
   } 

   function getHiddenEntryHTML($n, $v) 
   { 
      $n = htmlspecialchars($n, ENT_QUOTES); 
      $v = htmlspecialchars($v, ENT_QUOTES); 
      return "<input type='hidden' name='$n' value='$v'>"; 
   }

   function getListEntryField($option, &$type_obj, &$obj, $container, $jscript, &$vals, &$obj_set) 
   { 
      $f = array(); 
      if ( $option == "name" ) 
      { 
         $f[0] = "text"; 
         $onwards = $this->calling_req->copy(); 
         unset($onwards->target_objects); 
         $onwards->obj_type = array(strtolower($obj->getType())); 
         $onwards->site_id = array($obj_set->itemCollection[$obj->objref]->site_id); 
         $onwards->obj_id = $obj_set->itemCollection[$obj->objref]->site_id; 
         $onwards->action = 'view'; 
         $onwards->setTargetObjects(array($obj->id())); 
         $onwards->setEntryURL($this->calling_req->getAsEntryURL()); 
         $lnk = $onwards->getLink(); 
         $f[1] = "<a href='$lnk' title='view'>".$obj->name()."</a>"; 
      } 
      elseif ( $option == "id" ) 
      { 
         $f[0] = "number"; 
         $f[1] = $obj->id(); 
      } 
      elseif ( $option == "type" ) 
      { 
         $f[0] = "image"; 
         $f[1] = "<img align='middle' src='".$type_obj->getTypeIcon()."' alt='$type_obj->name' title='$type_obj->name'>"; 
      } 
      elseif ( $option == "active" ) 
      { 
         $f[0] = 'image'; 
         if($obj->isActive()) $f[1] = "<img src='graphics/active.gif' alt='active' title='$type_obj->name is shared'>"; 
         else $f[1] = "<img src='graphics/inactive.gif' alt='inactive' title='$type_obj->name is not shared'>"; 
      } 
      elseif ( $option == "protected" ) 
      { 
         $f[0] = 'image'; 
         if($obj->isProtected()) $f[1] = "<img src='graphics/active.gif' alt='protected' title='$type_obj->name is protected'>"; 
         else $f[1] = "<img src='graphics/inactive.gif' alt='not protected' title='$type_obj->name is not protected'>"; 
      } 
      elseif ( $option == "include" ) 
      { 
         if($vals['position'] > 0) $checked = "checked"; 
         else $checked = ''; 
         $f[0] = 'checkbox'; 
         $f[1] = "<input type=checkbox name='".$container."_include_".$obj->id()."' $jscript $checked>"; 
      } 
      elseif ( $option == "position" ) 
      { 
         $f[0] = 'numberbox'; 
         $f[1] = "<input type='text' size='2' maxlength='2' name='$container"."_position_".$obj->id()."' value='".$vals['position']."' ".$vals['enabled'].">"; 
      } 
      elseif ( $option == "style" ) 
      { 
         $f[0] = 'textbox'; 
         $f[1] = "<input type='text' size='12' maxlength='50' name='$container"."_style_".$obj->id()."' value='".$vals['style']."' ".$vals['enabled'].">"; 
      } 
      elseif ( $option == "depth" ) 
      { 
         $f[0] = 'select'; 
         $f[1] = $vals['depth']; 
      } 
      elseif ( $option == "hsize" ) 
      { 
         $f[0] = 'select'; 
         $f[1] = $vals['hselect']; 
      } 
      elseif ( $option == "surround" ) 
      { 
         $f[0] = 'checkbox'; 
         $f[1] = "<input type=checkbox name='".$container."_surround_".$obj->id()."' ".$vals['enabled']." ".$vals['surround'].">"; 
      } 
      elseif ($option == "override_protected") 
      { 
         $f[0] = 'checkbox'; 
         $f[1] = "<input type=checkbox name='".$container."_override_protected_".$obj->id()."' ".$vals['enabled']." ".$vals['override_protected'].">"; 
      } 
      elseif ( $option == "view" || $option == "edit" || $option == 'xmlview' ) 
      { 
         $f[0] = 'image'; 
         $onwards = $this->calling_req->copy(); 
         unset($onwards->target_objects); 
         $onwards->site_id = array($obj_set->itemCollection[$obj->objref]->site_id); 
         $onwards->obj_id = $obj_set->itemCollection[$obj->objref]->site_id; 
         $onwards->action = $option; 
         $onwards->setTargetObjects(array($obj->id())); 
         $lnk = $onwards->getLink(); 
         $f[1] = "<a href='$lnk'><img border=0 src='graphics/$option".".gif' /></a>"; 
      } 
      return $f;

   } 

   function getEmptyContainerRow($cols, &$type_obj, $container) 
   { 
      $str = "<tr class = 'object-listing'><td class='object-listing' colspan='$cols'>&nbsp;No ".$this->getContainerMessage($container, $type_obj->name)."&nbsp;</td></tr>\n"; 
      return $str; 
   }

   function getContainerMessage($container, $type_name) 
   { 
      $container = preg_replace('/^(.*)contained/', '$1', $container); 
      $insert_text = ($container != "") ? "the <i>$container</i> section of ": ""; 
      return   "Objects included in $insert_text"."this $type_name"; 
   }

   function getEmptyAvailableSection($cols, &$type_obj, $container) 
   { 
      $str = "<tr class = 'object-listing'><td class='object-listing' colspan='$cols'>&nbsp;None Listed - change filter settings&nbsp;</td></tr>\n"; 
      return $str; 
   }

   function getAvailableMessage($container, $type_name) 
   { 
      $container = preg_replace('/^(.*)contained/', '$1', $container); 
      $insert_text = ($container != "") ? "the <i>$container</i> section of ": ""; 
      return   "Add New Objects to $insert_text this $type_name"; 
   }

   function getListTableHeader() 
   { 
      return "<table class='object-listing'>\n"; 
   } 

   function getListTableFooter() 
   { 
      return "</table>\n"; 
   }

   function getListSectionHeader($cols, &$type_obj, $container, $help, $is_contained) 
   { 
      $str ="<tr class='object-listing-header'>\n<th class='object-listing-section' colspan='$cols'>"; 

      if($is_contained) $str .= $this->getContainerMessage($container, $type_obj->name); 
      else $str .= $this->getAvailableMessage($container, $type_obj->name);

      $str .= " ".getHelpHTML($help, true, true); 
      $str.=   "</th>\n</tr>\n"; 
      return $str; 
   } 

   function getListSectionSpacer($cols) 
   { 
      $str ="<tr class='object-listing-spacer'>\n<th class='object-listing-spacer' colspan='$cols'></th></tr>"; 
      return $str; 
   }

   function getContentAdditionSection($cols, $cnt, $help) 
   { 
      $str  = "<tr class='object-listing'>\n<th class='object-listing-section' colspan='$cols'>"; 
      $str .= "Or Add Site Content Elements Directly to the List"; 
      $str .= getHelpHTML($help, true, true); 
      $str .=   "</th>\n</tr>\n"; 
      $str .= "<tr class='object-listing'>\n<td class='object-listing-filter' colspan='$cols'>"; 
      $str .= "Add site content element of type: <select name='$cnt"."_add_content_type'>"; 
      $str .= "<option value='story'>Story</option>\n"; 
      $str .= "<option value='comment'>Comment</option>\n"; 
      $str .= "<option value='attachment'>Attachment</option>\n"; 
      $str .= "</select>\n"; 
      $str .= " ID: <input size='5' maxlength='10' type='text' name='$cnt"."_add_content_id' value=''>"; 
      $str .= "<input type='submit' name='$cnt"."_add_content' value='Add &gt; &gt;'>"; 
      $str .=   "</td>\n</tr>\n"; 
      return $str; 
   } 

   function getListColumnHeader($option, $is_first, $is_last) 
   { 
      $str = ""; 
      if($is_first) 
      { 
         $cls_str = "class='object-listing-index-left'"; 
      } 
      elseif($is_last) 
      { 
         $cls_str = "class='object-listing-index-right'"; 
      } 
      else 
      { 
         $cls_str = "class='object-listing-index'"; 
      }

      if ( $option == "name" ) $str .= "<th $cls_str>Name</th>"; 
      elseif ( $option == "id" ) $str .=  "<th $cls_str>ID</th>"; 
      elseif ( $option == "type" ) $str .=  "<th $cls_str>Type</th>"; 
      elseif ( $option == "active" ) $str .=   "<th $cls_str>Shared</th>" ; 
      elseif ( $option == "protected" ) $str .=  "<th $cls_str>Protected</th>"; 
      elseif ( $option == "include" ) $str .=   "<th $cls_str>Include</th>"; 
      elseif ( $option == "position" )$str .=   "<th $cls_str>Position</th>"; 
      elseif ( $option == "view" ) $str .= "<th $cls_str>View</th>"; 
      elseif ( $option == "xmlview" ) $str .= "<th $cls_str>XML</th>"; 
      elseif ( $option == "edit" ) $str .= "<th $cls_str>Edit</th>"; 
      elseif ( $option == "style" ) $str .=  "<th $cls_str>Style</th>"; 
      elseif ( $option == "depth" )$str .= "<th $cls_str>Display</th>"; 
      elseif ( $option == "hsize" ) $str .= "<th $cls_str>Hsize</th>"; 
      elseif ( $option == "surround" ) 
      { 
         if(strtolower($this->getType()) == 'menu') 
         { 
            $str .= "<th $cls_str>No Surround Box</th>"; 
         } 
         else 
         { 
            $str .= "<th $cls_str>Surround Box</th>"; 
         } 
      } 
      elseif ($option == "override_protected") $str.="<th $cls_str>Protect</th>"; 
      return $str; 
   } 

   function getListColumnHeaders ( &$options, $advancedOptions, $adv) 
   { 
      $str = "<tr class='object-listing-index'>\n";

      for($i = 0; $i<count($options); $i++) 
      { 
         $is_first = false; 
         $is_last = false; 
         if($i == 0) 
         { 
            $is_first = true; 
         } 
         if(!$adv && $i == count($options) -1) 
         { 
            $is_last = true; 
         } 
         $str .= $this->getListColumnHeader($options[$i], $is_first, $is_last); 
      }

      if( $adv ) 
      { 
         for($i = 0; $i<count($advancedOptions); $i++) 
         { 
            if($i == count($advancedOptions) -1) 
            { 
               $is_last = true; 
            } 
            $str .= $this->getListColumnHeader($advancedOptions[$i], false, $is_last); 
         } 
      } 
      $str .= "</tr>"; 
      return $str; 
   }


   function getDefaultDisplayMode($depth) 
   { 
      if($depth == FULL_DEPTH) $display = "Full Page"; 
      elseif($depth == SUMMARY_DEPTH) $display = "Long Title and Sub-title"; 
      elseif($depth == LONG_MENU_DEPTH) $display = "Long Title"; 
      else $display = "Short Title";

      return $display; 
   } 


   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false) 
   { 
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth); 
      $cls_str = ($cls == "") ? "" : "class='$cls'"; 
      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);

      $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name()); 
      $href = "href='".$this->getSelfLink()."'"; 
      $lnk = "$h<a $href $cls_str>"; 
      $lnk_end = "</a>$h_e";
      $title = $this->getMeta('shortlinktext');
      $ret = $this->getHeadHTML($cls_str, $wrap);

      if($display == "Icon And Title") 
      { 
         $ret .= $lnk.$img.$title.$lnk_end; 
      } 
      elseif($display == "Icon") 
      { 
         $ret .= $lnk.$img.$lnk_end; 
      } 
      elseif($display == "Short Title") 
      { 
         $ret .= $lnk.$title.$lnk_end; 
      } 
      elseif($display ==  "Title and List") 
      { 
         if($this->getMeta("fullpagelink") == "on") 
         { 
            $ret .= $lnk.$title.$lnk_end; 
         } 
         else 
         { 
            $ret .= $h.$title.$h_e; 
         } 
      } 
      elseif($display != "List, without title") 
      { 
         $long_title = $title;
         if($this->getMeta('longlinktext') != "") $long_title = $this->getMeta('longlinktext');
         if($display == "Full Page" or ($this->getMeta("fullpagelink") != "on" and $display != "Long Title")) 
         { 
            $ret .= $h.$long_title.$h_e; 
         } 
         else 
         { 
            $ret .= $lnk.$long_title.$lnk_end; 
         } 
      }

      if($display == 'Full Page') 
      { 
         $ret .= $this->getMeta("header"); 
      }

      if(($display == "List, without title" or $display == "Title and List" or $display ==       "Long Title and List" or $display == "Drop Down List" or $display == "Full Page") and isset($this->containedItems['contained'])) 
      { 
         $list_entries = array();

         foreach( $this->containedItems['contained'] as $item ) 
         { 
            $cls_on = ($item->getStyle() == "") ? $cls : $item->getStyle(); 
            $s = $item->getAsCallerSettings(); 
            if($depth == FULL_DEPTH) $next_depth = SUMMARY_DEPTH; 
            elseif($depth < LONG_LIST_DEPTH) $next_depth = LONG_LIST_DEPTH; 
            else $next_depth = SHORT_LIST_DEPTH; 

            if($display == 'Full Page') 
            { 
               $next_depth = SUMMARY_DEPTH; 
               unset($s['depth']); 
            }

            if(substr($item->id, 0, 8) == 'content_') 
            { 
               if($item->isORProtected()) 
               { 
                  $needs_protection= true; 
               }

               $site_id = $obj_set->itemCollection[$this->objref]->site_id; 
               $parts = explode("_", $item->id); 
               if(!isset($parts[2])) continue;

               $content_type = $parts[1]; 
               $content_id = $parts[2]; 
               $needs_protection = false;

               $obj = new indyContentObject($item->id, $content_type); 
               $content_obj = $this->getContentItem($content_type, $content_id); 
               if($content_obj === false) continue; 
               if(!$this->loadSiteObject($obj_set)) 
               { 
                  $listentry = "INTERNAL ERROR"; 
                  continue; 
               }

               $st = $this->site_object->getSiteSettings(); 
               $ar_id = $st["articleobject"]; 
               $listentry = $obj->getHTML($cls_on, $next_depth, $content_obj, $s, $ar_id, $site_id); 
            } 
            else 
            { 
               $obj = $obj_set->fetchObject($item->getId(), $this->getLangCode()); 
               if(!$obj) 
               { 
                  continue; 
               } 
               $obj_stub = $obj_set->itemCollection[$obj->objref];

               if( !$obj or !$obj_stub) 
               { 
                  $this->setUserMessages( $obj_set->getUserMessages() ); 
                  continue; 
               } 

               $needs_protection = false; 
               if($obj->isProtected() or $item->isORProtected()) 
               { 
                  $needs_protection= true; 
               }

               if(strtolower($obj->getType()) == 'site' && $next_depth == FULL_DEPTH) $next_depth = SUMMARY_DEPTH; 
               $listentry = $obj->getObjectEditingHTML($obj_set, "", ""); 
               $listentry .= $obj->getHTML($cls_on, $next_depth, $obj_set, $s, $live); 
               $site_id = $obj_stub->site_id; 
            }

            if($needs_protection && $live) 
            { 
               if(!$this->checkObjectAccess($site_id)) continue; 
            } 
            if($listentry != "") 
            { 
               $list_entries[] = $this->getListContentHTML($live, $listentry, $cls_str, $needs_protection, $site_id);
            } 
         }

         if(count($list_entries) > 0) 
         { 
            $tag = $this->getListWrapTags(); 
            if($tag != "") 
            { 
               $ret .= "<$tag $cls_str>\n".implode("", $list_entries)."</$tag>\n"; 
            } 
            else 
            { 
               $ret .= implode("", $list_entries)."\n"; 
            } 
         } 
      }

      if($display == 'Full Page') 
      { 
         $ret .= $this->getMeta("footer"); 
      } 
      $ret .= $this->getFootHTML($cls_str, $wrap); 
      return $ret; 
   }

   function getListWrapTags() 
   { 
      $l_type = $this->getMeta('bullettype'); 

      if($l_type == 'bullets') $tag = 'UL'; 
      elseif($l_type == 'numbers') $tag = 'OL'; 
      else $tag = ""; 
      return $tag; 
   }

   function getListContentHTML($live, &$le, $cls, $needs_protection, $site_id) 
   { 
      $l_type = $this->getMeta('bullettype');

      if($l_type == "bullets" or $l_type == 'numbers') 
      { 
         $html = "<LI $cls>$le</LI>\n"; 
      } 
      else 
      {
         if(stristr($le, "<UL") or stristr($le, "<OL"))
	 {
		$html = "$le\n";
	 }
	 else
	 {
		$html = "<P $cls>$le</P>\n";
	 }
      }

      if((!$live) && $needs_protection) 
      { 
         $html = $this->getProtectionHTMLHead($site_id) . $html. $this->getProtectionHTMLFoot(); 
      } 
      return $html; 
   } 
} 
?>
