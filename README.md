# Jak pobrać repozytorium?
`git clone https://github.com/Trenek/Individual-Project`  
`cd Individual-Project`  
`git submodule update --init --remote --recursive`  
Alternatywnie  
`git clone https://github.com/Trenek/Individual-Project --recursive`

# Jak skompilować kod?
`cmake -B build`  
`cmake --build build --target all`

# Jak uruchomić kod?
Po kompilacji pliki wykonywalne znajdują się w folderze `build/simulations`
