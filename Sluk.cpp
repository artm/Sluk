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

    ParticleSystem() {}

    double allele(Gene gene, int index, double def = 0.0) {
        return (gene.size() > index) ? gene[index] : def;
    }

    explicit ParticleSystem(int size, const Gene& gene)
    {
        double life_m = allele(gene,0,5.0), life_sd = allele(gene,1,2.5),
               pos_m = 0, pos_sd = allele(gene,2,1.0),
               vel_m = 0, vel_sd = allele(gene,3,1.0);
        life = ArrayXd::NormalRnd(size, life_m, life_sd);
        pos = ArrayX3d::NormalRnd(size, 3, pos_m,pos_sd);
        vel = ArrayX3d::NormalRnd(size, 3, vel_m,vel_sd);
    }

    void advance(double dt)
    {
        // trust the compiler to optimize this
        pos += vel * (dt * (life>0.0).cast<double>()).replicate<1,3>();
        life -= dt;
    }
};

class Sluk : public AppBasic {
    ParticleSystem m_partsys;
    Timer m_timer;
    CameraPersp m_cam;
    Arcball m_arcball;

    void genesis() {
        ParticleSystem::Gene gene;

        gene.push_back( u01_rnd(1,4) );
        gene.push_back( u01_rnd(.5,3) );
        gene.push_back( u01_rnd(1,3) );
        gene.push_back( u01_rnd(1,3) );

        m_partsys = ParticleSystem(50000, gene);
    }

    void setup() {
        genesis();
        m_cam.setPerspective( 90.0f, getWindowAspectRatio(), 0.01f, 50.0f );
        m_cam.lookAt(Vec3f(0,0,5), Vec3f(0,0,0));

        GLfloat fogColor[4]= {1,1,1,1};
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogfv(GL_FOG_COLOR, fogColor);
        glFogf(GL_FOG_DENSITY, 3e-2);
        glEnable(GL_FOG);

        glPointSize(5);
        float point_params[] = {1,0,1};
        glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, point_params);
    }
    void update() {
        m_timer.stop();
        float dt = m_timer.getSeconds();
        m_timer.start();

        m_partsys.advance(dt);
        if ((m_partsys.life > 0).count() < 1)
            genesis();
    }
    void draw() {
        gl::setMatrices( m_cam );
        gl::clear(Color(1,1,1), true);
        gl::color(0,0,0);

        gl::rotate( m_arcball.getQuat() );
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 3, GL_DOUBLE, 0, m_partsys.pos.data() );
        glDrawArrays(GL_POINTS, 0, m_partsys.pos.rows());
        glDisableClientState(GL_VERTEX_ARRAY);
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

};

CINDER_APP_BASIC( Sluk, RendererGl )
