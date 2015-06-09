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

/************************************************************************/
/* Class to represent the datatypes of indy objects                     */
/************************************************************************/

class indyDataType extends indyObject
{
   var $type;
   var $version;
   var $name;
   var $typeMetaInfo = array();
   //var $description;
   var $language;
   var $displayable;
   var $not_translated;
   var $itemList = array();
   var $templateList = array();
   var $is_stub = false;
   var $typefile;
   var $typebase;
   var $parser;
   var $inType = false;
   var $inItem = false;
   var $currentItem;
   var $currentTag;
   var $currentTagContents;

   function produceWikiDocumentation($target_file)
   {
      global $system_config;
      $txt =
"---++ $this->name

Version $this->version

---+++ Description

".$this->typeMetaInfo['description']."

---+++ Configuration

Below are the list of settings that the user can choose from for each configured data object of the $this->name module.

| *Configurable Settings* |||||
| *Number* | *Configurable Variable* | *Description* | *Input Type* | *Input Rules* |\n";
      $i = 1;
      foreach($this->itemList as $item)
      {
         if($item->type == 'divider')
         {
            $cntnt = str_replace("\n", " ", $item->getMeta("title"));
            $txt .= "| *".$cntnt . "* |||||\n";
            $descr = trim(str_replace("\n", " ", $item->getMeta("description")));
            if(strlen($descr) > 0)
            {
               $txt .= "| _".$descr . "_ |||||\n";
            }
         }
         else
         {
            $cntnt = $item->getMeta("description");
            $extra = $item->getMeta("help");
            $title = $item->getMeta("title");
            if(strpos($extra, "locale sensitive strftime") === 0)
            {
               $extra = "[[http://www.php.net/manual/en/function.strftime.php][see strftime manual page]]";
            }
            if(strlen(trim($extra)) > 0)
            {
               $cntnt .= " - ".$extra;
            }
            $cntnt = str_replace("\n", " ", $cntnt);
            //figure out the rules entry..
            $rule_str = "";
            $got_text_rule = false;
            if($item->type == 'text' or $item->type == 'textarea')
            {
               $rules = explode(",", $item->getMeta("rules"));
               if(in_array('mandatory', $rules)) $rule_str .= "Mandatory Field. ";
               else $rule_str .= "Optional Field. ";
               foreach($rules as $r)
               {
                  if(strpos($r, "numeric") === 0)
                  {
                     $limit_start = strpos($r, "(");
                     $limit_end = strpos($r, ")");
                     if($limit_start !== false and $limit_end !== false and $limit_end > $limit_start+1)
                     {
                        $num_str = substr($r, $limit_start+1, $limit_end-$limit_start-1);
                        $vals = explode(" ", $num_str);
                        if(isset($vals[0]) && $vals[0] !== "" && isset($vals[1]))
                        {
                            $rule_str .= "Numeric Input between $vals[0] and $vals[1]. ";
                            $got_text_rule = true;
                        }
                     }
                  }
               }
               if(!$got_text_rule && in_array('html', $rules)) $rule_str .= "HTML / PHP input allowed. ";
               elseif(!$got_text_rule) $rule_str .= "Plain Text Only. ";
            }
            elseif($item->type == 'checkbox')
            {
               $rule_str .= "On / Off";
            }
            elseif($item->type == 'radio')
            {
               $rule_str .= "Options: ".str_replace(",", ", ",$item->getMeta("options"));
            }
            elseif($item->type == 'select')
            {
               $rule_str .= "Numeric Option between: ".$item->getMeta("dimension1")." and ".$item->getMeta("dimension2");
            }
            elseif($item->type == 'reference')
            {
               $rule_str .= "A reference to a data object of type _".str_replace(",", " or ",$item->getMeta("referencedtypes"))."_ ";
            }
            elseif($item->type == 'multifilterselect')
            {
               $rule_str .= "A selection from the _".str_replace(",", " and ",$item->getMeta("filterfields"))."_ Tables ";
               $cntnt = "A choice of multiple categories from the site";
               $title = "Category Filter";
            }
            elseif($item->type == 'filterselect')
            {
               $rule_str .= "A selection from the _".str_replace(",", " and ",$item->getMeta("filterfields"))."_ Tables ";
               $cntnt = "A choice of single categories from the site.";
               $title = "Category Filter";
            }
            elseif($item->type == "object-listing")
            {
               $title = $item->name;
               $incl = $item->getMeta("includelist");
               if($incl != "")
               {
                  $cntnt = "Can include objects from the following modules: _"
                  .str_replace(",", ", ",$incl)."_ ";
               }
               else
               {
                  $cntnt = "Can include objects from any modules except: _".str_replace(",", ", ",$item->getMeta("excludelist"))."_ ";
               }
            }
            $txt .= "| ".$i++." | ". htmlspecialchars($title) . " | ".htmlspecialchars($cntnt)." | ".$item->type." | ".htmlspecialchars($rule_str)." |\n";
         }
      }

      $txt .=
      "
---+++ Display Modes

";
      //here we get the display modes...
      require_once $system_config->indyobject_code_dir."indy".strtolower($this->type).".inc";
      $objToCreate = "indy".ucfirst(strtolower($this->type))."Object";
      $newobj = eval(sprintf("return new %s( 1, \$this->type);", $objToCreate));
      $disps = $newobj->display_options;
      foreach($disps as $d)
      {
         $txt.= '   1. '.$d."\n";
      }

$txt .=
"
---+++ Bugs

---+++ To do List and requested features

--Oscailt Auto-generated Documentation ". strftime("%d %b %Y", time())."\n";
      $fp = fopen($target_file,"w");
      // locking file
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, $txt);
         // unlocking file
         flock($fp, LOCK_UN);
      }
      fclose($fp);
   }

   function indyDataType()
   {
   }

   function getItemByName($name)
   {
      foreach ( $this->itemList  as $item )
      {
         if($item->name == $name) return $item;
      }
      return false;
   }

   function & getItemsByType($type_name)
   {
      $items = array();
      foreach ( $this->itemList  as $item )
      {
         if(strtolower($item->type) == strtolower($type_name)) $items[] = $item;
      }
      return $items;
   }

   /************************************************************************/
   /* Integrate the type with the template (basicobject) type              */
   /************************************************************************/
   function integrateTemplates()
   {
      for($i=0;$i<count($this->templateList);$i++)
      {
         $templ = $this->templateList[$i];
         $has = false;
         foreach($this->itemList as $item)
         {
            if($item->name == $templ->name)
            {
               $has = true;
               break;
            }
         }
         if(!$has)
         {
            if($templ->name == 'name' or $templ->name == 'objectcontents' or $templ->name == 'shortdescription' or $templ->name=='image')
            {
               array_unshift($this->itemList, $templ);
            }
            else
            {
               $this->itemList[] = $templ;
            }
         }

      }
   }

   function setMeta($n, $v)
   {
      $this->typeMetaInfo[$n] = $v;
   }

   function getMeta($n)
   {
      if(isset($this->typeMetaInfo[$n])) return $this->typeMetaInfo[$n];
      return false;
   }

   /************************************************************************/
    /* Loads the data from the data type set file  */
    /* Output:success/failure t/f */
    /************************************************************************/
   function load($fname, $templatefile)
   {
      $this->typefile = $fname;
      if( !file_exists( $this->typefile ) )
      {
         $this->setUserMessage(array(INDY_ERROR,
              "The data type file does not exist.  No file: ".$this->typefile,
              ""));
         return FALSE;
      }
      $xmlData = implode('', file($this->typefile));
      if(($xmlData !== FALSE) && ($this->readXML($xmlData, false) !== FALSE))
      {
         if($templatefile === false or !file_exists($templatefile))
         {
            return true;
         }
         $tmplData = implode('', file($templatefile));
         if(($tmplData !== FALSE) && ($this->readXML($tmplData, true) !== FALSE))
         {
            return true;
         }
         else
         {
            $this->setUserMessage( array( INDY_ERROR,
             "Failed to read the datatype from the template file: ".$templatefile,
             "asd" ) );
            return FALSE;
         }
      }
      else
      {
         $this->setUserMessage( array( INDY_ERROR,
              "Failed to read the datatype from the file: ".$this->typefile,
              "a3s") );
         return FALSE;
      }
      return true;
   }

   function readXML($source, $is_template = false)
   {
        if (!function_exists('xml_parser_create'))
        {
            $this->setUserMessage( array( INDY_ERROR, "Failed to load PHP's XML Extension. " .
                          "http://www.php.net/manual/en/ref.xml.php","" ) );
            return false;
        }

        $parser = @xml_parser_create();

        if ( !is_resource( $parser ) )
        {
            $this->setUserMessage( array( INDY_ERROR, "Failed to create an instance of PHP's XML parser. " .
                  "http://www.php.net/manual/en/ref.xml.php","" ) );
            return false;
        }

        $this->parser = $parser;
        xml_set_object( $this->parser, $this );
        if(!$is_template)
        {
           xml_set_element_handler($this->parser, 'start_element', 'end_element');
        }
        else
        {
          xml_set_element_handler($this->parser, 'templatestart_element', 'templateend_element');
        }

        xml_set_character_data_handler( $this->parser, 'cdata' );
        xml_set_processing_instruction_handler ( $this->parser, 'pi_handler' );
        $status = xml_parse( $this->parser, $source );
        if (! $status )
        {
            $errorcode = xml_get_error_code( $this->parser );
            if ( $errorcode != XML_ERROR_NONE )
            {
                $xml_error = xml_error_string( $errorcode );
                $error_line = xml_get_current_line_number($this->parser);
                $error_col = xml_get_current_column_number($this->parser);
                $errormsg = "$xml_error at line $error_line, column $error_col";
                $this->setUserMessage( array( INDY_ERROR,  "XML error: ".$errormsg, "" ) );
                return false;
            }
        }
        xml_parser_free( $this->parser );
        return true;
    }


   function templatestart_element($p, $element, &$attrs)
   {
        $element = strtolower( $element );
        if ( $element == 'indytype' )
        {
            $this->inType = true;
        }
        elseif (! $this->inType )
        {
            return;
        }
        elseif ( $element == 'item' )
        {
            $this->inItem = true;
            $item_name = ( isset( $attrs['NAME'] ) ) ? $attrs['NAME'] : "";
            $item_type = ( isset ( $attrs['TYPE'] ) ) ? $attrs['TYPE'] : "";
            if($item_name != "" and $item_type != "")
            {
               $this->currentItem = new indyDataItem($item_name, $item_type);
            }
        }
        else
        {
            $this->currentTag = $element;
            $this->currentTagContents = "";
        }

    }

    function start_element ($p, $element, &$attrs)
    {
        $element = strtolower( $element );
        if ( $element == 'indytype' )
        {
            $this->inType = true;
            $this->type = $attrs['TYPE'];
            $this->name = $attrs['NAME'];
            $this->version = $attrs['VERSION'];
            $this->language = $attrs['LANGUAGE'];
            if( isset ( $attrs['DISPLAYABLE'] ) && $attrs['DISPLAYABLE']  == "yes" )
            {
                $this->displayable = true;
            }
            else $this->displayable = false;
        }
        elseif (! $this->inType )
        {
            return;
        }
        elseif ( $element == 'item' )
        {
            $this->inItem = true;
            $item_name = ( isset( $attrs['NAME'] ) ) ? $attrs['NAME'] : "";
            $item_type = ( isset ( $attrs['TYPE'] ) ) ? $attrs['TYPE'] : "";
            if($item_name != "" and $item_type != "")
            {
               $this->currentItem = new indyDataItem($item_name, $item_type);
            }
        }
        else
        {
            $this->currentTag = $element;
            $this->currentTagContents = "";
        }
    }

    function templateend_element($p, $element)
    {
        $element = strtolower($element);
        if ( $element == 'indytype' )
        {
            $this->inType = FALSE;
            unset($this->currentItem);
            $this->integrateTemplates();
        }
        elseif ( $element == 'item' )
        {
            $this->templateList[] = $this->currentItem;
            $this->inItem = false;
         unset($this->currentItem);
        }
        elseif( $this->inItem && isset( $this->currentTag ) && $element == $this->currentTag)
        {
             $this->currentItem->setMeta($this->currentTag, trim($this->currentTagContents));
         $this->currentTagContents = "";
        }
        elseif($this->inType)
        {
           if(!isset($this->typeMetaInfo[$this->currentTag]))
           {
              $this->setMeta($this->currentTag, $this->currentTagContents);
           }
           $this->currentTagContents = "";
        }

    }

    function end_element ($p, $element)
    {
        $element = strtolower($element);

        if ( $element == 'indytype' )
        {
            $this->inType = FALSE;
            unset($this->currentItem);
        }
        elseif ( $element == 'item' )
        {
            $this->itemList[] = $this->currentItem;
            $this->inItem = false;
         unset($this->currentItem);
        }
        elseif( $this->inItem && isset( $this->currentTag ) && $element == $this->currentTag)
        {
            $this->currentItem->setMeta($this->currentTag, trim($this->currentTagContents));
            $this->currentTagContents = "";
        }
        elseif($this->inType)
        {
           $this->setMeta($this->currentTag, $this->currentTagContents);
           $this->currentTagContents = "";
        }
    }

    function cdata ($p, $text)
    {

        if($this->inType)
        {
            $this->currentTagContents .= $text;
        }
    }

    /************************************************************************/
    /* executes any php code in the xml                                     */
    /************************************************************************/
    function pi_handler($p, $target, $text)
    {
       if(strtolower($target) != 'php')
       {
          return;
       }
       elseif($this->inType)
       {
          ob_start();
          eval($text);
          $buffered_output = ob_get_contents();
          ob_end_clean();
          $this->currentTagContents .= $buffered_output;
       }
   }

   /************************************************************************/
   /* Is this type directly displayable? (menu, bar, banner, page, box)
   /* inputs: a type name
   /* output: true/false
   /************************************************************************/
   function isDisplayable ()
   {
      return $this->displayable;
   }

   /************************************************************************/
   /* Does this type contain other objects?
   /* inputs: a type name
   /* output: true/false
   /************************************************************************/
   function isComposite ()
   {
      foreach ( $this->itemList  as $item )
      {
         if($item->type == "object-listing") return true;
      }
      return false;
   }

   function getReferenceFields ()
   {
      $fields = array();
      foreach ( $this->itemList  as $item )
      {
         if($item->type == "reference")
         {
            $fields[] = $item->name;
         }
      }
      return $fields;
   }

   function getListingFields ()
   {
      $fields = array();
      foreach ( $this->itemList  as $item )
      {
         if($item->type == "object-listing") $fields[] = $item->name;
      }
      return $fields;
   }

   function getCategoryRefFields()
   {
      $fields = array();
      foreach ( $this->itemList  as $item )
      {
         if($item->type == "multifilterselect" or $item->type == "filterselect")
         {
            $fs = explode(",",$item->metadata['filterfields']);
            foreach($fs as $f)
            {
               $fields[$item->name.$f] = $f;
            }
         }
      }
      return $fields;
   }

   /************************************************************************/
   /* reads the information about the form to input the object */
   /* inputs: none
   /* output: an array where each entery in the array is a field of the form
   /*      that is needed to input the data and some other details
   /************************************************************************/
   function getTypeInfo()
   {

      $retinfo = array();
      $retinfo['description'] = $this->getMeta('description');
      //cycle through the fields of the map them to html-able types, and store them in an array.
      foreach ($this->itemList as $item)
      {
         $ret = array();
         if($item->type == "select")
         {
            $ret = $item->metadata;
            $ret['type'] = $item->type;
            $ret['name'] = $item->name;
            if($item->getMeta("dimension1") == "" or $item->getMeta("dimension2") == "" or ($item->getMeta("dimension1") >= $item->getMeta("dimension2")))
            {
               continue;
            }
            if($item->getMeta("dimension3") > 0)
            {
               $stepsize = $item->getMeta("dimension3");
               unset($ret["dimension3"]);
            }
            else $stepsize = 1;
            for($i=$item->getMeta("dimension1"); $i<=$item->getMeta("dimension2"); $i=$i+$stepsize)
            {
               $ret["options"][$i] = $i;
            }
            //don't want to pass these back to form-drawing code
            unset ($ret["dimension1"]);
            unset ($ret["dimension2"]);
         }
         elseif ( $item->type == "multifilterselect" || $item->type == "filterselect")
         {
            $ffs = explode(",",$item->metadata['filterfields']);
            $startp = $item->metadata['helpstart'];
            $endp = $item->metadata['helpend'];
            $this->setUpListings($ffs, $ret, $startp, $endp, $item->type);
            $ret['name'] = $item->name;
            $ret['specialtype'] = $item->type;
         }
         else
         {
            $ret = $item->metadata;
            $ret['type'] = $item->type;
            $ret['name'] = $item->name;//was overwritten
         }
         $retinfo[] = $ret;
      }
      return $retinfo;
   }

   /************************************************************************/
   /* returns the validation rules for each input field of the type
   /* inputs: none
   /* output: an array where each entry in the array is a fieldname -> rule list pair
   /************************************************************************/
   function getValidationInfo()
   {
       $validation_info = array();
       foreach ($this->itemList as $item)
       {
          //input types that are cross-checked independently
          if ( $item->type == "multifilterselect" || $item->type == "filterselect" or $item->type == 'reference' or $item->type == 'object-listing' or $item->type == 'checkbox')
          {
             continue;
          }
          if($item->type == "select")
          {
             if($item->getMeta("dimension1") == "" or $item->getMeta("dimension2") == "" or ($item->getMeta("dimension1") >= $item->getMeta("dimension2")))
             {
                continue;
             }
             if($item->getMeta("dimension3") > 0)
             {
                $stepsize = $item->getMeta("dimension3");
             }
             else $stepsize = 1;
             $vals = array();
             for($i=$item->getMeta("dimension1"); $i<=$item->getMeta("dimension2"); $i=$i+$stepsize)
             {
                 $vals[] = $i;
             }
             if(count($vals) > 0)
             {
                $validation_info[$item->name] = array('list' => $vals, 'title' => $item->getMeta("title"));
             }
          }
          elseif( $item->type == 'radio')
          {
             $vals = array();
             $options = $item->getMeta('options');
             if($options === false) continue;
             $all_options = explode(",", $options);
             foreach($all_options as $opt)
             {
                $opt = trim($opt);
                if($opt != "") $vals[] = $opt;
             }
             if(count($vals) == 0) continue;
             $validation_info[$item->name] = array('list' => $vals, 'title' => $item->getMeta("title"));
          }
          elseif($item->type == 'text' or $item->type == 'textarea')
          {
             $rules = array();
             $rulestr = $item->getMeta('rules');
             if($rulestr !== false)
             {
                $rule_bits = explode(",", $rulestr);
                foreach($rule_bits as $r)
                {
                   $r = trim($r);
                   if($r != "") $rules[] = $r;
                }
             }
             $validation_info[$item->name] = array('rules' => $rules, 'title' => $item->getMeta("title"));
          }
      }
      return $validation_info;
   }

   function getTypeIcon($size = "small")
   {
      if($this->getMeta('icon'.$size) === false){
         return 'graphics/mediaicons/32x32/diy.png';
      }
      else return $this->getMeta('icon'.$size);
   }


   function getListingInfo ($container_name, $area="")
   {
      if ( !$this->isComposite()) return false;
      $info = array();
      $container_item = $this->getItemByName($container_name);
      if($container_item === false) return false;
      $info['includedhelp'] = $container_item->getMeta('includedhelp');
      $info['availablehelp'] = $container_item->getMeta('availablehelp');
      $info['contentshelp'] = $container_item->getMeta('contentshelp');
      $exclu = $container_item->getMeta('excludelist');
      if( isset ( $exclu ) && $exclu != "")
      {
         $info['excludes'] = explode( ",",$exclu);
      }
      $inclu = $container_item->getMeta('includelist');
      if( isset ( $inclu ) && $inclu != "")
      {
         $info['includes'] = explode( ",", $container_item->getMeta('includelist') );
      }
      if( $container_item->getMeta('options') != "" )
      {
         $info['options'] = explode( ",", $container_item->getMeta('options') );
      }
      if( $container_item->getMeta('advancedoptions') != "" )
      {
         $info['advancedoptions'] = explode( ",", $container_item->getMeta('advancedoptions') );
      }
      return $info;
   }

   /************************************************************************/
   /* Creates the drop down listing boxes for filters on topics etc.
   /************************************************************************/
   function setUpListings ($fields, &$arr, $s, $e, $ty)
   {
      global $topicList,$regionList,$languageList, $typeList;

      foreach ($fields as $f)
      {
         $arr[$f]['title'] = $f;
         $arr[$f]['help'] = "$s $f $e";
         $arr[$f]['options']['None'] = "All ".ucfirst($f)."s";

         if($f == "topic")
         {
            $topics=$topicList->getTopics();
            for ($i=count($topics)-1; $i>=0; $i--)
            {
               $arr[$f]['options'][$topics[$i]->topic_id] = $topics[$i]->topic_name;
            }
         }
         elseif ($f == "region")
         {
            $regions=$regionList->getRegions();
            for ($i=count($regions)-1; $i>=0; $i--)
            {
               $arr[$f]['options'][$regions[$i]->region_id] = $regions[$i]->region_name;
            }
         }
         elseif ($f == "language")
         {
            $languages=$languageList->getLanguages();
            for ($i=count($languages)-1; $i>=0; $i--)
            {
               $arr[$f]['options'][$languages[$i]->getStrippedCode()] = $languages[$i]->language_name;
            }
         }
         elseif ($f == "type")
         {
            $types=$typeList->getTypes();
            for ($i=count($types)-1; $i>=0; $i--)
            {
               $arr[$f]['options'][$types[$i]->type_id] = $types[$i]->type_name;
            }
         }
      }
   }
}







/************************************************************************/
/* This class is a collection of all available data object types */
/************************************************************************/
class indyDataTypeSet extends indyObject
{
   var $dataTypes = array();
   var $legalTypes = array();
   var $typeDetails = array();
   var $basicType;
   var $legaltypefile;
   var $datatypedir;

   function indyDataTypeSet ( $datatypedir )
   {
      $this->datatypedir = $datatypedir;
   }

   function typecount()
   {
      return count($this->legalTypes);
   }

   function load()
   {
      global $legalDataTypes;
      $this->legalTypes = array_keys($legalDataTypes);
      array_unshift($this->legalTypes, 'basic');
      if(!$this->loadType('basic'))
      {
        $this->setUserMessage(array(INDY_ERROR,"Failed to load info for basic type.",""));
        return false;
      }
      $this->basicType = $this->dataTypes['basic'];
      unset($this->dataTypes['basic']);
      array_shift($this->legalTypes);
      return true;
   }

   function getTypes(){
      return $this->dataTypes;
   }

   function getStructuralTypes(){
      $ret = array();
      foreach($this->dataTypes as $t){
         if($t->isDisplayable()){
            $ret[] = $t;
         }
      }
      return $ret;
   }

   function getContainableTypes(){
      $ret = array();
      foreach($this->dataTypes as $t){
         if(!$t->isDisplayable()){
            $ret[] = $t;
         }
      }
      return $ret;
   }

   function loadAllTypes()
   {
       $some_worked = false;
       foreach($this->legalTypes as $onetype)
       {
          if(!$this->loadType($onetype))
          {
            $this->setUserMessage(array(INDY_ERROR,
             "Failed to load info for type $onetype",""));
           }
          else $some_worked = true;
       }
       return $some_worked;

   }

   function loadType($t, $lang_code = "")
   {
      global $performance_test;
      $t = strtolower( $t );
      //first check for the type name in my data types set...
      if(!in_array($t, $this->legalTypes)){
         $this->setUserMessage(array(INDY_ERROR,
             "The data type does not exist.  No type ".$t." in set of legal types!",""));
            return false;
      }
      $typefile = getFileByLanguage($this->datatypedir.'/', $t, $lang_code);
      $templatefile = getFileByLanguage($this->datatypedir.'/', 'basic', $lang_code);

      if($typefile === false)
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to open non-existent file $typefile in directory $this->datatypedir","" ) );
         return false;
      }
      if($t == 'basic')
      {
         $use_template=false;
      }
      else
      {
         $use_template=$templatefile;
      }
      $dataType = new indyDataType();
      if($dataType->load($typefile, $templatefile))
      {
         if($lang_code == "")
         {
            $dataType->not_translated = ! compareToUserLanguage($dataType->language);
         }
         else
         {
            $dataType->not_translated = ($dataType->language != $lang_code);
         }
         $this->dataTypes[$dataType->type] =& $dataType;
         $this->typeDetails[$t] =& $this->dataTypes[$dataType->type];
         if($performance_test > 3)  markTime("Loaded Data Type $t");
         return true;
      }
      $this->setUserMessages($dataType->getUserMessages());
      return false;
   }

   function unloadType($t)
   {
      $t = strtolower($t);
      unset($this->dataTypes[$t]);
   }

   /************************************************************************/
   /* returns the available types as html options...
   /* output: html string with <option value=x>typename</option>
   /************************************************************************/
   function getTypesAsOptions($types)
   {
      $str = "";
      foreach($this->dataTypes as $type)
      {
         if(in_array($type->type, $types)) $sel = "selected";
         else $sel = "";

         $str .= "<option $sel value='$type->type'>$type->name</option>\n";
      }
      return $str;
   }

   /************************************************************************/
   /* reads the information about the form to input the object */
   /* inputs: a type name
   /* output: an array where each entery in the array is a field of the form
   /*      that is needed to input the data and some other details
   /************************************************************************/
   function getTypeInfo($t)
   {
      $t = strtolower( $t );
      if(!in_array($t, array_keys($this->dataTypes))){
         if(!$this->loadType($t)){
            $this->setUserMessage(array(INDY_ERROR,
                "Failed to load info for type $t","asd"));
               return false;
         }
      }
      return $this->dataTypes[$t]->getTypeInfo();
   }

   /************************************************************************/
   /* Returns an array of the fields that are needed for input             */
   /************************************************************************/
   function getInputFields( $t )
   {
      $allfields = $this->getTypeInfo($t);
      $ret = array();
      unset($allfields['description']);
      foreach($allfields as $f)
      {
         if(isset($f['type'])) $is_ip = $this->isInputField($f['type']);
         else $is_ip = false;
         if(isset($f['specialtype']))
         {
            $specialname = $f['name'];
            unset($f['specialtype']);
            unset($f['name']);
            foreach(array_keys($f) as $one_special)
            {
               $ret[] = $specialname.$one_special;
            }
         }
         elseif($is_ip)
         {
            $ret[] = $f['name'];
         }
      }
      return $ret;
   }


   function getValidationRules($t)
   {
      $t_obj = $this->getType($t);
      if($t_obj === false) return false;
      return $t_obj->getValidationInfo();
   }

   function getTypeName($t)
   {
      $ty = $this->getType($t);
      if($ty)
      {
         return $ty->name;
      }
      return false;
   }

   function getType( $t, $lang_code = "" )
   {
      $t = strtolower( $t );
      if(!in_array($t, array_keys($this->dataTypes))){
         if(!$this->loadType($t, $lang_code)){
            $this->setUserMessage(array(INDY_ERROR,
                "Failed to load info for type $t",""));
               return false;
         }
         else
         {
            $this->setUserMessage(array(DEBUG_LEVEL_2,
                         "Loading Type: $t not already loaded","342"));
               //return false;
         }
      }
      return $this->dataTypes[$t];
   }


   /************************************************************************/
   /* Is this type directly displayable? (menu, bar, vbar, banner, page)
   /* inputs: a type name
   /* output: true/false
   /************************************************************************/
   function isDisplayable ( $t )
   {
      $t = strtolower( $t );
      if(!in_array($t, array_keys($this->dataTypes))){
         if(!$this->loadType($t)){
            $this->setUserMessage(array(INDY_ERROR,
               "Failed to load info for type $t",""));
            return false;
         }
      }
      return $this->dataTypes[$t]->isDisplayable();
   }

   /************************************************************************/
   /* Does this type contain other objects?
   /* inputs: a type name
   /* output: true/false
   /************************************************************************/
   function isComposite ( $t )
   {
      $t = strtolower( $t );
      if(!in_array($t, array_keys($this->dataTypes))){
         if(!$this->loadType($t)){
            $this->setUserMessage(array(INDY_ERROR,
               "Failed to load info for type $t",""));
            return false;
         }
      }
      return $this->dataTypes[$t]->isComposite();
   }

   function getListingInfo ( $t, $container, $offset="" )
   {
      $t = strtolower( $t );
      if(!in_array($t, array_keys($this->dataTypes))){
         if(!$this->loadType($t)){
            $this->setUserMessage(array(INDY_ERROR,
               "Failed to load info for type $t",""));
            return false;
         }
      }
      return $this->dataTypes[$t]->getListingInfo($container, $offset);
   }

   function isInputField($t)
   {
      if($t == 'text' or $t == 'textarea' or $t == 'select' or $t == 'filterselect' or $t == 'multifilterselect' or $t == 'checkbox' or $t == 'radio' or $t == 'reference' or $t== 'object-listing' ) return true;
      return false;
   }

   function saveTypeDetailsCache()
   {
      global $system_config, $userLanguage, $languageList;
      foreach($this->legalTypes as $l_type)
      {
         $langs = $languageList->languages;
         foreach($langs as $l)
         {
            $lang_code = $l->getLanguageCodePrefix();
            $target_file = $system_config->type_details_store.$l_type."_".$lang_code.".cache";
            //$source_file = getFileByLanguage($system_config->type_details_store, $t, "", ".cache");
            $full_type_obj = $this->getType($l_type, $lang_code);
            if($full_type_obj)
            {
               $stub_obj = new indyDataType();
               $stub_obj->type = $full_type_obj->type;
               $stub_obj->name = $full_type_obj->name;
               $stub_obj->version = $full_type_obj->version;
               $stub_obj->typeMetaInfo =& $full_type_obj->typeMetaInfo;
               $stub_obj->language = $lang_code;//$full_type_obj->language;
               $stub_obj->displayable = $full_type_obj->displayable;
               $stub_obj->typefile = $full_type_obj->typefile;
               $stub_obj->typebase = $full_type_obj->typebase;
               $stub_obj->not_translated = $full_type_obj->not_translated;
               $stub_obj->is_stub = true;
               $txt = serialize($stub_obj);
               $fp = fopen($target_file,"w");
               // locking file
               if (flock($fp, LOCK_EX))
               {
                  fputs($fp, $txt);
                  // unlocking file
                  flock($fp, LOCK_UN);
               }
               fclose($fp);
               if($lang_code == $system_config->default_language_code)
               {
                  $target_file = $system_config->type_details_store.$l_type.".cache";
                  $fp = fopen($target_file,"w");
                  if (flock($fp, LOCK_EX))
                  {
                     fputs($fp, $txt);
                     // unlocking file
                     flock($fp, LOCK_UN);
                  }
                  fclose($fp);
               }
               $this->unloadType($l_type);
            }
         }
      }
   }

   function loadAllTypeDetails()
   {
      global $userLanguage;
      foreach($this->legalTypes as $l_type)
      {
         if(!isset($this->typeDetails[$l_type]))
         {
            if($this->loadTypeDetails($l_type) === false)
            {
               $type = $this->getType($l_type);
               if(!$type)
               {
                  return false;
               }
            }
         }
      }
      return true;
   }

   function getAllTypeDetails()
   {
      return array_values($this->typeDetails);
   }


   function loadTypeDetails($t)
   {
      global $system_config;
      $t = strtolower($t);
      $source_file = getFileByLanguage($system_config->type_details_store, $t, "", ".cache");
      if(file_exists($source_file))
      {
         $file = file($source_file);
         $txt = implode("", $file);
         $this->typeDetails[$t] = unserialize($txt);
         return true;
      }
      return false;
   }

   function getTypeDetails($t)
   {
      $t = strtolower($t);
      if(!isset($this->typeDetails[$t]))
      {
         if(!$this->loadTypeDetails($t))
         {
            if(!$this->getType($t))
            {
               return false;
            }
         }
      }
      return $this->typeDetails[$t];
   }

   /************************************************************************/
   /* returns an array of all types in the set */
   /************************************************************************/
   function getSupportedTypes()
   {
      return $this->legalTypes;
   }

   function supportsType($t)
   {
      $t = strtolower($t);
      return in_array($t, $this->legalTypes);
   }

   /************************************************************************/
   /* Function for producing the documentation about the data modules */
   /************************************************************************/
   function produceDocumentation()
   {
      $this->loadAllTypes();
      foreach($this->dataTypes as $type)
      {
         $fname = "xmldata/documentation/".$type->type.".wiki";
         $type->produceWikiDocumentation($fname);
      }
   }
}
?>