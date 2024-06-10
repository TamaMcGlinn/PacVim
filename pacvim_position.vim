function! PlaceMonster() abort
  call appendbufline(bufname(), '$', GetMonsterDef())
endfunction

function! GetMonsterDef() abort
  let [_, lnum, cnum, _] = getpos('.')
  let monster_definition = "/.5 " . (cnum-1) . ' ' . (lnum-1)
  return monster_definition
endfunction

nnoremap <silent> <C-m> :call PlaceMonster()<CR>
