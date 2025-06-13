cmake_minimum_required(VERSION 3.27)
project(The_Last_Warden)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(Resource)

file(COPY Resource/audios DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(The_Last_Warden
    Player/Player.cpp 
    Player/Player.hpp 
    Player/Spearman.cpp 
    Player/Spearman.hpp 
    Player/Companion.cpp
    Player/Companion.hpp
    Player/Mage.cpp
    Player/Mage.hpp
    Player/Archer.cpp
    Player/Archer.hpp

    Weapon/Weapon.cpp 
    Weapon/Weapon.hpp 
    Weapon/SpearWeapon.cpp 
    Weapon/SpearWeapon.hpp
    Weapon/WandWeapon.cpp
    Weapon/WandWeapon.hpp
    Weapon/BowWeapon.cpp
    Weapon/BowWeapon.hpp

    Projectile/Projectile.cpp
    Projectile/Projectile.hpp
    Projectile/Arrow.cpp 
    Projectile/Arrow.hpp 
    Projectile/MagicBullet.cpp 
    Projectile/MagicBullet.hpp 
    Projectile/ArrowBullet.cpp 
    Projectile/ArrowBullet.hpp 

    Enemy/Enemy.cpp
    Enemy/Enemy.hpp
    Enemy/GreenSlime.cpp 
    Enemy/GreenSlime.hpp
    Enemy/ToxicSlime.cpp 
    Enemy/ToxicSlime.hpp
    Enemy/LavaSlime.cpp 
    Enemy/LavaSlime.hpp
    Enemy/Orc.cpp 
    Enemy/Orc.hpp
    Enemy/HighOrc.cpp 
    Enemy/HighOrc.hpp

    Engine/Allegro5Exception.cpp
    Engine/Allegro5Exception.hpp
    Engine/AudioHelper.cpp
    Engine/AudioHelper.hpp
    Engine/Collider.cpp
    Engine/Collider.hpp
    Engine/GameEngine.cpp
    Engine/GameEngine.hpp
    Engine/Group.cpp
    Engine/Group.hpp
    Engine/IControl.cpp
    Engine/IControl.hpp
    Engine/IObject.cpp
    Engine/IObject.hpp
    Engine/IScene.cpp
    Engine/IScene.hpp
    Engine/LOG.cpp
    Engine/LOG.hpp
    Engine/Point.cpp
    Engine/Point.hpp
    Engine/Resources.cpp
    Engine/Resources.hpp
    Engine/Sprite.cpp
    Engine/Sprite.hpp

    Scene/PlayScene.cpp
    Scene/PlayScene.hpp
    Scene/ModeSelectScene.cpp
    Scene/ModeSelectScene.hpp
    Scene/SettingScene.cpp
    Scene/SettingScene.hpp
    Scene/LeaderBoardScene.cpp
    Scene/LeaderBoardScene.hpp
    Scene/AuthScene.cpp
    Scene/AuthScene.hpp
    Scene/RegisterScene.cpp
    Scene/RegisterScene.hpp
    Scene/LoginScene.cpp
    Scene/LoginScene.hpp
    Scene/SelectHeroScene.cpp
    Scene/SelectHeroScene.hpp
    Scene/SelectCompanionScene.cpp
    Scene/SelectCompanionScene.hpp
    Scene/ScoreScene.cpp
    Scene/ScoreScene.hpp
    Scene/SplashScene.cpp
    Scene/SplashScene.hpp

    Structure/Structure.cpp 
    Structure/Structure.hpp
    Structure/StructureButton.cpp
    Structure/StructureButton.hpp
    Structure/Offense/Tower.cpp
    Structure/Offense/Tower.hpp
    Structure/Offense/BowTower.cpp 
    Structure/Offense/BowTower.hpp 
    Structure/Defense/SmashBone.cpp
    Structure/Defense/SmashBone.hpp
    Structure/Defense/Axe.cpp
    Structure/Defense/Axe.hpp
    Structure/Defense/Wall.cpp 
    Structure/Defense/Wall.hpp 
    Structure/Defense/BasicWall.cpp 
    Structure/Defense/BasicWall.hpp 

    UI/Animation/DirtyEffect.cpp
    UI/Animation/DirtyEffect.hpp
    UI/Animation/ExplosionEffect.cpp
    UI/Animation/ExplosionEffect.hpp
    UI/Animation/AreaEffect.cpp
    UI/Animation/AreaEffect.hpp
    UI/Animation/Cheat.cpp
    UI/Animation/Cheat.hpp
    UI/Animation/ZukoKnockbackWave.cpp
    UI/Animation/ZukoKnockbackWave.hpp

    UI/Component/Image.cpp
    UI/Component/Image.hpp
    UI/Component/ImageButton.cpp
    UI/Component/ImageButton.hpp
    UI/Component/Label.cpp
    UI/Component/Label.hpp
    UI/Component/Slider.cpp
    UI/Component/Slider.hpp
    
    main.cpp
)

IF (WIN32)
    target_include_directories(The_Last_Warden PUBLIC C:/Application/C++/Allegro/allegro/include)
    target_link_directories(The_Last_Warden PUBLIC C:/Application/C++/Allegro/allegro/lib)
    target_compile_options(The_Last_Warden PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(The_Last_Warden liballegro_monolith)
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(The_Last_Warden PUBLIC lib)
    target_include_directories(The_Last_Warden PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(The_Last_Warden PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(The_Last_Warden ${ALLEGRO5_LIBRARIES})
    target_link_directories(The_Last_Warden PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
ENDIF()