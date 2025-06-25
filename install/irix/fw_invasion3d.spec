product fw_invasion3d
    id "OpenGL Space Invaders!"
    image sw
        id "Software"
        version 2
        order 9999
        subsys eoe default
            id "Invasion3D executable and data"
            replaces self
            exp fw_invasion3d.sw.eoe
        endsubsys
    endimage
    image opt
        id "Optional"
        version 2
        order 9999
        subsys relnotes default
            id "Release Notes"
            replaces self
            exp fw_invasion3d.opt.relnotes
        endsubsys
    endimage
endproduct
