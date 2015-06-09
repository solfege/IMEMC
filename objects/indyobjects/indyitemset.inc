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
/* An item is just a collection of data from an xml list                */
/************************************************************************/
class indyDataItem
{
   var $name;
   var $type;
   var $metadata = array();

   function indyDataItem($n, $t){
      $this->name = $n;
      $this->type = $t;
   }

   function setMeta($n, $v){
      $this->metadata[$n] = $v;
   }

   function getMeta($n){
      if(isset($this->metadata[$n])) return $this->metadata[$n];
      return false;
   }

   function getXML()
   {
      $str = "<item name='$this->name' type='$this->type'>\n";
      foreach($this->metadata as $k => $v)
      {
         $str .= "<$k>$v</$k>\n";
      }
      $str .= "</item>\n";
      return $str;
   }

   function getIcon($size = "")
   {
      if($this->getMeta('icon'.$size) == ""){
         return 'graphics/mediaicons/32x32/diy.png';
      }
      else return $this->getMeta('icon'.$size);
   }

}

/************************************************************************/
/* Class to represent collections of xml itesm                          */
/************************************************************************/
class indyItemSet extends indyObject
{
   var $name;
   var $language;
   var $not_translated;
   var $itemSetMetaInfo = array();
   var $itemList = array();
   var $itemfile;

   var $parser;
   var $inItemSet = false;
   var $inItem = false;
   var $currentItem;
   var $currentTag;
   var $currentTagContents;

   function indyItemSet()
   {
   }

   function getItemsByType($type)
   {
      $typs = array();
      foreach ( $this->itemList  as $item )
      {
         if($item->type == $type) $typs[] = $item;
      }
      return $typs;
   }

   function getItemByName($name)
   {
      foreach ( $this->itemList  as $item )
      {
         if($item->name == $name) return $item;
      }
      return false;
   }

   function getString($type_name)
   {
      $item = $this->getItemByName($type_name);
      if(!$item) return "";
      return $item->getMeta("title");
   }

   function removeItemName($name)
   {
      foreach ( array_keys($this->itemList)  as $item_k )
      {
         if($this->itemList[$item_k]->name == $name)
         {
            unset($this->itemList[$item_k]);
            return true;
         }
      }
      return false;
   }

   function addItem($item)
   {
      $this->itemList[] = $item;
   }

   function getItems ()
   {
      return $this->itemList;
   }

   function setMeta($n, $v)
   {
      $this->itemSetMetaInfo[$n] = $v;
   }

   function getMeta($n)
   {
      if(isset($this->itemSetMetaInfo[$n])) return $this->itemSetMetaInfo[$n];
      return false;
   }

   /************************************************************************/
    /* Loads the data from the data type set file  */
    /* Output:success/failure t/f */
    /************************************************************************/
   function load($xmldocbase, $xmldocname)
   {
      $itemfile = getFileByLanguage($xmldocbase, $xmldocname);
      if($itemfile === false)
      {
         $this->setUserMessage(array(INDY_ERROR,      "The item set file for $xmldocname does not exist in $xmldocbase",""));
         return false;
      }

      $this->itemfile = $itemfile;
      $xmlData = implode('', file($this->itemfile));

        if(($xmlData !== FALSE) && ($this->readXML($xmlData) !== FALSE))
        {
         $this->not_translated = ! compareToUserLanguage($this->language);
            return TRUE;
        }
        else
        {
            $this->setUserMessage( array( INDY_ERROR,
                "Failed to read the item set from the file: ".$this->itemfile,
                "421" ) );
            return FALSE;
        }
   }

   function save($xmldocbase)
   {
      $contents = $this->getXML();
      $itemfile = getFileByLanguage($xmldocbase, $this->name);
      if($itemfile == false or !file_exists($itemfile))
      {
         $itemfile = $xmldocbase.$this->name.".xml";
      }
      $fp = fopen($itemfile,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, $contents);
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else
      {
         reportError("<P>failed to get lock on export metadata file $itemfile</p>");
         return false;
      }
      return true;
   }

   function getXMLHeader(){
      $xmlStr = '<?xml version="1.0" encoding="ISO-8859-1" ?>';
      return $xmlStr;
   }

    /************************************************************************/
    /* Returns the data object set as an XML string  */
    /* Input: none */
    /* Output:string */
    /************************************************************************/
    function getXML ()
    {
       $str = $this->getXMLHeader();
       $str .= "\n<indyItemSet name='".$this->name."' language='".$this->language."'>\n";
       foreach($this->itemSetMetaInfo as $k => $v)
       {
         $str .= "<$k>$v</$k>\n";
       }
       foreach($this->itemList as $item)
       {
         $str .= $item->getXML();
       }
       $str .= "</indyItemSet>\n";
       return $str;
    }



   function readXML($source)
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
        xml_set_element_handler($this->parser, 'start_element', 'end_element');
        xml_set_character_data_handler( $this->parser, 'cdata' );

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


    function start_element ($p, $element, &$attrs)
    {
        $element = strtolower( $element );
        if ( $element == 'indyitemset' )
        {
            $this->inItemSet = true;
            $this->name = $attrs['NAME'];
            $this->language = $attrs['LANGUAGE'];
        }
        elseif (! $this->inItemSet )
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


    function end_element ($p, $element)
    {
        $element = strtolower($element);

        if ( $element == 'indyitemset' )
        {
            $this->inItemSet = FALSE;
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
        elseif($this->inItemSet)
        {
           $this->setMeta($this->currentTag, $this->currentTagContents);
        }
    }

    function cdata ($p, $text)
    {
        if($this->inItemSet)
        {
            $this->currentTagContents .= $text;
        }
    }

    function getIcon($size = "small")
    {
       if($this->getMeta('icon'.$size) === false){
          return 'graphics/mediaicons/32x32/diy.png';
       }
       else return $this->getMeta('icon'.$size);
    }
  }
?>