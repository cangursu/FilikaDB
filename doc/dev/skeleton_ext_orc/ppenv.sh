

alias cl='sudo make clean distclean uninstall'
alias rb='cl && make' 
alias in='cl && sudo make all install'
alias test='make installcheck'
alias all='cl && in && test'
alias db='pg_ctl -D /usr/local/pgsql/data -l ~/logfile restart'

