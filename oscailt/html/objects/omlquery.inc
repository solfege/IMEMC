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
/* This program is distributed in the hope that it will be useful ,     */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/
require_once 'objects/magpie/snoopy.inc';

class OMLOutline
{
   var $parent;
   var $attributes;
   var $items = array();
   var $outlines = array();
   var $cdata;
}

class OMLOutlineItem
{
   var $attributes;
   var $cdata;
}

class OMLParser
{
   var $parser;
   var $currentOutline=null;
   var $currentItem=null;
   var $outlines = array();
   var $ERROR = "";
   var $convert_from_utf8=false;

   function OMLParser ($source, $convert_from_utf8=false)
   {
      # if PHP xml isn't compiled in, die
      #
      $this->convert_from_utf8 = $convert_from_utf8;

      if (!function_exists('xml_parser_create')) {
         $this->error( "Failed to load PHP's XML Extension. " .
                    "http://www.php.net/manual/en/ref.xml.php",
                     E_USER_ERROR );
      }
      $parser = @xml_parser_create();
      if (!is_resource($parser))
      {
         $this->error( "Failed to create an instance of PHP's XML parser. " .
                    "http://www.php.net/manual/en/ref.xml.php",
                    E_USER_ERROR );
      }
      $this->parser = $parser;
      # pass in parser, and a reference to this object
      # setup handlers
      #
      xml_set_object( $this->parser, $this );

      // In versions higher than PHP 5.0 (e.g. 5.1), it defaults to UTF-8 which surprisenly doesn't handle
      // ISO-8859-1 character data very well. Storing result from set in case we need to log it sometime.
      // But there is also a bug sometimes wheret the option below will not set and returns false.
      $t_res = xml_parser_set_option($this->parser, XML_OPTION_TARGET_ENCODING, 'ISO-8859-1');
      if ($t_res === false) {
	   global $system_config;
	   if (phpversion() >=5 ) {
		if ($system_config->user_error_reporting > 1 )
		    logMessage("Switching to utf8 conversion for oml", "sitelog.txt");
	        $this->convert_from_utf8 = true;
	   } else {
		if ($system_config->user_error_reporting > 1 )
		    logMessage("Set encoding option returned okay for oml parser", "sitelog.txt");
	   }
      }

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
            $this->error( $errormsg );
         }
      }
      xml_parser_free( $this->parser );
   }

   function start_element ($p, $element, &$attrs)
   {
      $element = strtolower( $element );
      if ( $element == 'outline' )
      {
         $outline = new OMLOutline();
         $outline->attributes=$attrs;
         $outline->parent=$this->currentOutline;
         $this->currentOutline = $outline;
         $this->currentElement = $outline;
      }
      else if ( $element == 'item' )
      {
         $item = new OMLOutlineItem();
         $item->attributes=$attrs;
         $this->currentItem = $item;
         $this->currentElement = $item;
      }
   }

   function end_element ($p, $element)
   {
      $element = strtolower($element);
      if ( $element == 'outline' )
      {
         if($this->currentOutline->parent==null)
         {
            array_push($this->outlines,$this->currentOutline);
            $this->currentOutline=null;
         }
         else
         {
            array_push($this->currentOutline->parent->outlines,$this->currentOutline);
            $this->currentOutline=$this->currentOutline->parent;
         }
      }
      else if ( $element == 'item' )
      {
         array_push($this->currentOutline->items,$this->currentItem);
      }
   }

   function cdata($p, $text)
   {
      if($this->convert_from_utf8 == true) {
          $t_text = iconv("UTF-8","ISO-8859-1", $text);
	  if ($t_text === FALSE) {
	      $t_text = $text;
	      global $system_config;
	      if ($system_config->user_error_reporting > 1 )
		    logMessage("iconv failure in UTF-8 conversion for oml", "sitelog.txt");
	  }

          if($this->currentItem != null)
          {
             $this->currentItem->cdata = $this->currentItem->cdata.$t_text;
          }
          else if($this->currentOutline != null)
          {
             $this->currentOutline->cdata = $this->currentOutline->cdata.$t_text;
          }
      } else {
          if($this->currentItem != null)
          {
             $this->currentItem->cdata=$this->currentItem->cdata.$text;
          }
          else if($this->currentOutline != null)
          {
             $this->currentOutline->cdata=$this->currentOutline->cdata.$text;
          }
      }
   }

   function error($errormsg, $lvl=E_USER_WARNING)
   {
      global $php_errormsg;
      // append PHP's error message if track_errors enabled
      if ( $php_errormsg ) {
         $errormsg .= " ($php_errormsg)";
      }
      $this->ERROR = $errormsg;
      trigger_error( $errormsg, $lvl);
   }

   // debugging functions
   function show_outlines()
   {
      echo "<ul>";
      foreach ($this->outlines as $outline)
      {
         echo($this->show_outline( $outline ));
      }
      echo "</ul>";
   }

   function show_outline(&$outline)
   {
      echo("<li>Outline");
      echo "<ul>";
      while ( list($key, $value) = each($outline->attributes) )
      {
         echo "<li> $key : $value";
      }
      echo "<li>cdata : ".$outline->cdata;
      foreach ($outline->items as $item)
      {
         echo $this->show_item( $item );
      }
      foreach ($outline->outlines as $outline)
      {
         echo $this->show_outline( $outline );
      }
      echo "</ul>";
   }

   function show_item ($item)
   {
      echo "<li>Item";
      echo "<ul>";
      while ( list($key, $value) = each($item->attributes) )
      {
         echo "<li> $key: $value";
      }
      echo "<li>cdata : ".$item->cdata;
      echo "</ul>";
   }
} # end class OML

class OMLQuery
{
   var $convert_from_utf8;
   var $install_mode = false;

   function OMLQuery($convert_from_utf8=false)
   {
       $this->convert_from_utf8 = $convert_from_utf8;
   }

   function setInstallMode()
   {
       $this->install_mode = true;
   }
   function fetchRawOML($url)
   {
      global $system_config;
      // Snoopy is an HTTP client in PHP
      $client = new Snoopy();
      $client->agent = $system_config->software_name.'/'. $system_config->software_version ;
      $client->read_timeout = 20;
      $client->use_gzip = true;
      @$client->fetch($url);
      return $client;
   }

   function fetchParsedOML($url)
   {
      $resp = $this->fetchRawOML($url);
      if (isset($resp) and $resp)
      {
         if ( $this->is_success( $resp->status ) )
         {
            $oml = new OMLParser( $resp->results, $this->convert_from_utf8 );
            if(!$oml->ERROR) return $oml;
            else $errormsg = "Failed to parse oml. ";
         }
         else
         {
            $errormsg = "Failed to fetch oml at $url";
            if ( $resp->error ) {
               # compensate for Snoopy's annoying habbit to tacking
               # on '\n'
               $http_error = substr($resp->error, 0, -2);
               $errormsg .= "(HTTP Error: $http_error)";
            }
            else
            {
               $errormsg .=  "(HTTP Response: " . $resp->response_code .')';
            }
         }
      }
      else
      {
         $errormsg = "Unable to retrieve OML file for unknown reasons.";
      }
      if ($this->install_mode == true )
          echo("<ul><li class='install-error'>Warning: ".$errormsg."</li></ul>");
      else
          echo($errormsg);

      return null;
   }
   function is_success ($sc)
   {
      return $sc >= 200 && $sc < 300;
   }
}
?>
