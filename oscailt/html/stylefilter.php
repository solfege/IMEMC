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
include "objects/utilities.inc";

$fontIncrement=cleanseNumericalQueryField($_REQUEST['fontsizeinc']);
if($fontIncrement>0)
{
   header("Content-Type: text/css");
   //prevent malicious loading of non stylesheet files
   $file_extension = strrchr($_REQUEST['style'], ".");
   if($file_extension==".css")
   {
      $fcontents = file ($_REQUEST['style']);
      $fct = implode("", $fcontents);
      $search = array("/FONT-SIZE:\s*([0-9]*)px/sie","/(BODY[^\}]+)FONT-SIZE:\s*([0-9]*)\.?([0-9]*)em([^\}]*\})/sie");
      $replace = array("'FONT-SIZE: '.('\\1'+$fontIncrement).'px'", "'\\1'.'FONT-SIZE: '.((('\\2'*10)+'\\3'+$fontIncrement)/10).'em'.'\\4'");
      echo(preg_replace($search, $replace,$fct));
      //while (list ($line_num, $line) = each ($fcontents))
      //{
      //   $search = array("/FONT-SIZE:\s*([0-9]*)px/sie","/BODY.*FONT-SIZE:\s*([0-9]*)\.?([0-9]*)em/si");
         //$replace = array("'FONT-SIZE: '.('\\1'+$fontIncrement).'px'", "'\\1'.'FONT-SIZE: '.((('\\2'*10)+'\\3'+$fontIncrement)/10).'em'.'\\4'");
         //$search = array("/FONT-SIZE:\s*([0-9]*)px/sie","/FONT-SIZE:\s*([0-9]+)\.?([0-9]*)em/sie");
      //   $replace = array("'FONT-SIZE: '.('\\1'+$fontIncrement).'px'", "hello");
      //   echo(preg_replace($search, $replace,$line));
      //}
   }
}
else
{
   Header("Location: ".$_REQUEST['style']);
}
?>
