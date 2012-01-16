#include <cinder/app/AppBasic.h>
#include <cinder/Camera.h>
#include <cinder/Arcball.h>

#include "RandomUtil.hpp"
#define EIGEN_ARRAYBASE_PLUGIN <ArrayBasePlugin.hpp>
#include <Eigen/Core>

#include "RandomUtilEigen.hpp"

using namespace ci;
using namespace ci::app;

using Eigen::ArrayX3d;
using Eigen::ArrayXd;

struct ParticleSystem {
    typedef std::vector<double> Gene;

    ArrayX3d pos, vel;
    ArrayXd life;

    double life_m, life_sd, pos_sd, vel_sd;

    ParticleSystem() {}

    double allele(Gene gene, int index, double def = 0.0) {
        return (gene.size() > index) ? gene[index] : def;
    }

    explicit ParticleSystem(int size, const Gene& gene)
    {
        life_m = allele(gene,0,5.0);
        life_sd = allele(gene,1,2.5);
        pos_sd = allele(gene,2,1.0);
        vel_sd = allele(gene,3,1.0);

        life = ArrayXd::Zero(size);
        pos = ArrayX3d::Zero(size,3);
        vel = ArrayX3d::Zero(size,3);
    }

    void advance(double dt)
    {
        int size = life.size();

        vel = (life > 0.0).replicate<1,3>()
            .select( vel, ArrayX3d::NormalRnd(size, 3, 0.0, vel_sd) );
        pos = (life > 0.0).replicate<1,3>()
            .select( pos + vel * dt, ArrayX3d::NormalRnd(size, 3, 0.0, pos_sd) );
        life = (life > 0.0)
            .select( life - dt, ArrayXd::NormalRnd(size, life_m, life_sd) );
    }
};

class Sluk : public AppBasic {
    ParticleSystem m_partsys;
    Timer m_timer;
    CameraPersp m_cam;
    Arcball m_arcball;

    void genesis() {
        ParticleSystem::Gene gene;

        gene.push_back( u01_rnd(.1,4) );
        gene.push_back( u01_rnd(.1,5) );
        gene.push_back( u01_rnd(.1,3) );
        gene.push_back( u01_rnd(.1,3) );

        m_partsys = ParticleSystem(15000, gene);
    }

    void setup() {
        genesis();
        m_cam.setPerspective( 90.0f, getWindowAspectRatio(), 0.01f, 500.0f );
        m_cam.lookAt(Vec3f(0,0,-10), Vec3f(0,0,0));

        GLfloat fogColor[4]= {0,0,0,1};
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogfv(GL_FOG_COLOR, fogColor);
        glFogf(GL_FOG_DENSITY, 7e-3);
        glEnable(GL_FOG);

        float psize = 20;
        glPointSize(psize);
        float point_params[] = {1,0,.5};
        glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
        glPointParameterf(GL_POINT_SIZE_MAX, psize);
        glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, point_params);

        glBlendFunc(GL_ONE, GL_ONE);
    }
    void update() {
        m_timer.stop();
        float dt = m_timer.getSeconds();
        m_timer.start();

        m_partsys.advance(dt);
        if ((m_partsys.life < 0).all())
            genesis();
    }
    void draw() {
        gl::setMatrices( m_cam );
        gl::clear(Color(0,0,0), true);
        gl::color(0.1,0.1,0.1);

        gl::rotate( m_arcball.getQuat() );

        glEnable(GL_BLEND);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 3, GL_DOUBLE, 0, m_partsys.pos.data() );
        glDrawArrays(GL_POINTS, 0, m_partsys.pos.rows());
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_BLEND);
    }

    void resize(ResizeEvent e) {
        m_arcball.setCenter( getWindowCenter() );
        m_arcball.setRadius( getWindowSize().y / 2 );
        m_cam.setAspectRatio(e.getAspectRatio());
    }

    void mouseDown( MouseEvent e )
    {
        m_arcball.mouseDown( e.getPos() );
    }

    void mouseDrag( MouseEvent e )
    {
        m_arcball.mouseDrag( e.getPos() );
    }

    void keyDown( KeyEvent e )
    {
        switch(e.getChar()) {
            case ' ':
                genesis();
                break;
        }
    }

};

CINDER_APP_BASIC( Sluk, RendererGl )
