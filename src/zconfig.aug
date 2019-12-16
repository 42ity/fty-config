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

let data = key Rx.word . ( Sep.space_equal . ( Quote.do_dquote ( store /[^\n"]*/ ) ) )? . comment?

let dataL0 = data . eol
let dataL1 = indent . data . eol
let dataL2 = indent . indent . data . eol
let dataL3 = indent . indent . indent . data . eol
let dataL4 = indent . indent . indent . indent . data . eol

(*let dataL0 = data . eol
let dataL1 = indent . data . eol
let dataL2 = indent . indent . data . eol
let dataL3 = indent . indent . indent . data . eol
let dataL4 = indent . indent . indent . indent . data . eol*)

let treeL4 = [ dataL4 ]
let treeL3 = [ dataL3 . treeL4* ]
let treeL2 = [ dataL2 . treeL3* ]
let treeL1 = [ dataL1 . treeL2* ]
let treeL0 = [ dataL0 . treeL1* ]

let lns = ( Util.comment | empty | treeL0 )+

let filter =
  incl "/etc/fty/*.cfg" .
  incl "/etc/etn-*/*.cfg" .
  incl "/etc/fty-*/*.cfg" .
  incl "/var/lib/fty/*/*.cfg" .
  Util.stdexcl

let xfm = transform lns filter
