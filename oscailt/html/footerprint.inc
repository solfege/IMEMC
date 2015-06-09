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
$link=$_SERVER['REQUEST_URI'];
$link = preg_replace("/(\?|&)print_page=true*/","",$link);
$link = preg_replace("/(\?|&)include_comments=true*/","",$link);
$link = htmlspecialchars($link, ENT_QUOTES);
?>

<FONT face=arial,helvetica color=#333333><small><center><b>http://<?=$system_config->site_url?><?=$link?></b></center><BR><?=$system_config->site_description?><BR>
</FONT></small>
<?
disconnectFromDatabase();
?>
</BODY>
</HTML>
