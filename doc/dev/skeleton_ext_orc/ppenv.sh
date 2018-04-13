

alias cl='sudo make clean distclean uninstall'
alias rb='cl && make' 
alias in='cl && sudo make all install'
alias test='make installcheck'
alias all='cl && sudo make all install && make installcheck'

