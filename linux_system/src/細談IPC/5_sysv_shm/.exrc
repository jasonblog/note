if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
inoremap <silent> <S-Tab> =BackwardsSnippet()
snoremap <silent> 	 i<Right>=TriggerSnippet()
snoremap  b<BS>
nnoremap <silent> w :CCTreeWindowToggle
nnoremap <silent> y :CCTreeWindowSaveCopy
snoremap % b<BS>%
snoremap ' b<BS>'
snoremap U b<BS>U
snoremap \ b<BS>\
nmap \ihn :IHN
nmap \is :IHS:A
nmap \ih :IHS
snoremap ^ b<BS>^
snoremap ` b<BS>`
nmap gx <Plug>NetrwBrowseX
snoremap <Left> bi
snoremap <Right> a
snoremap <BS> b<BS>
snoremap <silent> <S-Tab> i<Right>=BackwardsSnippet()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
nnoremap <silent> <F11> :call conque_term#exec_file()
nnoremap <silent> <F8> : A
nnoremap <silent> <F6> : Tlist
nnoremap <silent> <F5> : NERDTree
nnoremap <silent> <F3> : Rgrep
nnoremap <silent> <F2> : w
inoremap <silent> 	 =TriggerSnippet()
inoremap <silent> 	 =ShowAvailableSnips()
imap \ihn :IHN
imap \is :IHS:A
imap \ih :IHS
let &cpo=s:cpo_save
unlet s:cpo_save
set background=dark
set backspace=indent,eol,start
set backup
set cindent
set completeopt=longest,menu
set cscopeprg=/usr/bin/cscope
set cscopetag
set cscopeverbose
set expandtab
set fileencodings=ucs-bom,utf-8,latin1
set foldlevelstart=99
set guicursor=n-v-c:block,o:hor50,i-ci:hor15,r-cr:hor30,sm:block,a:blinkon0
set helplang=en
set history=50
set hlsearch
set laststatus=2
set listchars=tab:>-,trail:-
set nomodeline
set mouse=a
set ruler
set shiftwidth=4
set statusline=%m\ %t\ format=%{&ff}\ file_type=%y\ ASCII=[%4b/0x%-3B]\ line=[%4l/%-4L\ %%%-2p]\ column=%c
set tabstop=4
set viminfo='20,\"50
" vim: set ft=vim :
