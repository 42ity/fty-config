(*
Module: Zconfig

Author: Jean-Baptiste Boric <Jean-BaptisteBORIC@eaton.com>
*)
module Zconfig =

autoload xfm

let indent = del /    / "    "
let eol = del /\n/ "\n"

let comment = [ label "#comment" . del / +#/ " #" . store /[^\n]*/ ]
let empty = Util.empty

let header = key Rx.word . (del / = ""/ " = \"\"") ? . comment? . eol
let data = Quote.dquote_spaces ( key Rx.word . del / = / " = " ) . comment? . eol

let headerL0 = header
let headerL1 = indent . header
let headerL2 = indent . indent . header

let dataL1 = ( indent . data )
let dataL2 = ( indent . indent . data )
let dataL3 = ( indent . indent . indent . data )

let treeL2 = [ headerL2 . (dataL3)* ]
let treeL1 = [ headerL1 . (dataL2 | treeL2)* ]
let treeL0 = [ headerL0 . (dataL1 | treeL1)* ]

let lns = (Util.comment | empty | treeL0 | data )*

let filter =
  incl "/etc/default/fty.cfg" .
  incl "/etc/fty/*.cfg" .
  incl "/etc/etn-*/*.cfg" .
  incl "/etc/fty-*/*.cfg" .
  Util.stdexcl

let xfm = transform lns filter
