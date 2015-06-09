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

$code_delimiters = array(
            '<?',
            '?>',
            '<%',
            '%>');

$evil_tags = array(
            "html",
            "title",
            "layer",
            "javascript",
            "java",
            'head',
            "script",
            "object",
            "iframe",
            "image",
            "applet",
            "meta",
            "form",
            "body",
            "frame",
            "frameset",
            "embed",
            "base",
            "textarea",
            "input",
            "option",
            "select");

$evil_attributes = array(
            "javascript",
            "onabort",
            "onactivate",
            "onafterprint",
            "onbeforeactivate",
            "onbeforeeditfocus",
            "onbeforepaste",
            "onbeforeprint",
            "onbeforeunload",
            "onbegin",
            "onblur",
            "onchange",
            "onclick",
            "oncontextmenu",
            "oncopy",
            "oncut",
            "ondblclick",
            "ondeactivate",
            "ondragdrop",
            "ondragend",
            "ondragenter",
            "ondragleave",
            "ondragover",
            "onend",
            "onerror",
            "onexit",
            "onhelp",
            "onkeydown",
            "onkeypress",
            "onkeyup",
            "onlayoutcomplete",
            "onload",
            "onmousedown",
            "onmouseenter",
            "onmouseleave",
            "onmousemove",
            "onmouseover",
            "onmouseout",
            "onmouseup",
            "onmousewheel",
            "onmove",
            "onmoveend",
            "onmovestart",
            "onpaste",
            "onpropertychange",
            "onreadystatechange",
            "onreset",
            "onresize",
            "onresizesend",
            "onresizestart",
            "onscroll",
            "onselect",
            "onselectstart",
            "onselectionchange",
            "onsubmit",
            "onunload"
);

if (isset($load_wysiwyg_editor) && $load_wysiwyg_editor == 2) {

    $public_tags = array('strong', 'small', 'span', 'em', 'b', 'u', 'ul', 'li', 'br', 'p', 'div', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6');
    $public_attributes = array('span' => array('class','style'), 'div' => array('class','style'));

} else {

    $public_tags = array('strong', 'small', 'em', 'b', 'u', 'ul', 'li', 'p');
    $public_attributes = array('span' => array('class','style'));
}


$basic_tags = array(
      'a',
      'b',
      'i',
      'strong',
      'small',
      'em',
      'u',
      'p',
      'hr',
      'tt',
      'h1', 'h2', 'h3', 'h4', 'h5',
      'img');

$basic_attributes = array(
      'a' => array('href','title','name'),
      'h1' => array('align'),
      'h2' => array('align'),
      'h3' => array('align'),
      'h4' => array('align'),
      'h5' => array('align'),
      'img' => array('alt', 'src', 'align', 'width', 'height', 'border', 'hspace', 'vspace')
);

// The use of approved tags is set by one of the admin permissions called 'Use Approved HTML in forms'
// The actual set allowed are a merged combination of these and basic_tags and public_tags

$approved_tags= array(
            'br',
            'ul',
            'li',
            'ol',
            'p',
            'a',
            'h1', 'h2', 'h3', 'h4', 'h5',
            'img',
            'table', 'thead', 'tbody', 'tr', 'td', 'th',
            'div',
            'hr',
            'span',
            'big',
            'sup',
            's',
            'sub',
            'blockquote',
            'pre',
            'dl',
            'dt',
            'dd',
            'cite',
            'address',
            'center',
            'style'
);


$approved_attributes = array(
            'a' => array('class','style','href','title','name'),
            'address' => array('class','style'),
            'b' => array('class','style'),
            'big' => array('class','style'),
            'blockquote' => array('class','style'),
            'br' => array('class','style','clear'),
            'center' => array('class','style'),
            'cite' => array('class','style'),
            'dd' => array('class','style'),
            'div' => array('class','style', 'align'),
            'dl' => array('class','style'),
            'dt' => array('class','style'),
            'em' => array('class','style'),
            'h1' => array('class','style','align'),
            'h2' => array('class','style', 'align'),
            'h3' => array('class','style', 'align'),
            'h4' => array('class','style', 'align'),
            'h5' => array('class','style', 'align'),
            'hr' => array('class','style', 'align', 'width'),
            'i' => array('class','style'),
            'img' => array('class','style', 'alt', 'src', 'align', 'width', 'height', 'border', 'hspace', 'vspace'),
            'li' => array('class','style','type', 'value'),
            'ol' => array('class','style', 'type', 'start'),
            'p' => array('class','style', 'align', 'clear'),
            'pre' => array('class','style'),
            's' => array('class','style'),
            'small' => array('class','style'),
            'span' => array('class','style'),
            'strong' => array('class','style'),
            'style' => array(),
            'sup' => array('class','style'),
            'sub' => array('class','style'),
            'table' => array('class','style','align', 'width', 'height', 'border', 'hspace', 'vspace','bgcolor', 'background','cellpadding','cellspacing'),
            'tbody' => array('class','style'),
            'thead' => array('class','style'),
            'tr' => array('class','style','align', 'valign', 'halign','bgcolor', 'background'),
            'td' => array('class','style','align', 'valign', 'width', 'height', 'rowspan', 'colspan','bgcolor', 'background'),
            'th' => array('class','style','align', 'valign', 'width', 'height', 'rowspan', 'colspan','bgcolor', 'background'),
            'tt' => array('class','style'),
            'u' => array('class','style'),
            'ul' => array('class','style', 'type'),
);


// Oscailt only supports bbcode that can't carry injections and can't fubar formatting.
$allowed_bb_code = array(
         'b',
         'i',
         'quote',
         'list');

// The html tags that are permitted in summary views (note this is only for display
// other tags are not stripped from the content
$newswire_summary_display_tags = array(
      'a',
      'strong',
      'cite',
      'emp',
      'b',
      'i',
      'u',
      'br'
);
?>
