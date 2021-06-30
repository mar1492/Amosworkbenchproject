#include "utils.hpp"

#include <QTableWidgetItem>

#if _MSC_VER || __MINGW32__
    #include <windows.h>
    #include <psapi.h>
#elif defined(__APPLE__)
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <sys/vmmeter.h>
#else
    #include <sys/sysinfo.h>
#endif

// #define ARMA_DONT_USE_LAPACK

using namespace arma;
using namespace mitk;

bool Utils::showTime = false;
LogSender *Utils::logSender = 0;
ScalarType Utils::epsQform = 1.0e-6;

void Utils::cubePointerToSegmentation(Image::Pointer image, arma::Cube<unsigned char>* cube)
{
    ImageWriteAccessor accessor(image);
//     void* vPointer = accessor.GetData();
    unsigned char *mPointer = static_cast<unsigned char*>(accessor.GetData());
    *cube = arma::Cube<unsigned char>(mPointer, image->GetDimension(0), image->GetDimension(1), image->GetDimension(2), false, true);
}

void Utils::nifti_mat_to_orientation(itk::Matrix<ScalarType> R, int *icod, int *jcod, int *kcod)
{
    ScalarType xi,xj,xk , yi,yj,yk , zi,zj,zk , val, detQ, detP ;
    itk::Matrix<ScalarType> P, Q, M;
    int i,j,k=0,p,q,r , ibest,jbest,kbest,pbest,qbest,rbest ;
    ScalarType vbest ;

    if( icod == NULL || jcod == NULL || kcod == NULL )
        return ; /* bad */

    *icod = *jcod = *kcod = 0 ; /* error returns, if sh*t happens */

    /* load column vectors for each (i,j,k) direction from matrix */

    /*-- i axis --*/ /*-- j axis --*/ /*-- k axis --*/

    xi = R(0, 0) ;
    xj = R(0, 1) ;
    xk = R(0, 2) ;
    yi = R(1, 0) ;
    yj = R(1, 1) ;
    yk = R(1, 2) ;
    zi = R(2, 0) ;
    zj = R(2, 1) ;
    zk = R(2, 2) ;

    /* normalize column vectors to get unit vectors along each ijk-axis */

    /* normalize i axis */

    val = sqrt( xi*xi + yi*yi + zi*zi ) ;
    if( val == 0.0 )
        return ;                 /* stupid input */
    xi /= val ;
    yi /= val ;
    zi /= val ;

    /* normalize j axis */

    val = sqrt( xj*xj + yj*yj + zj*zj ) ;
    if( val == 0.0 )
        return ;                 /* stupid input */
    xj /= val ;
    yj /= val ;
    zj /= val ;

    /* orthogonalize j axis to i axis, if needed */

    val = xi*xj + yi*yj + zi*zj ;    /* dot product between i and j */
    if( fabs(val) > 1.e-4 ) {
        xj -= val*xi ;
        yj -= val*yi ;
        zj -= val*zi ;
        val = sqrt( xj*xj + yj*yj + zj*zj ) ;  /* must renormalize */
        if( val == 0.0 )
            return ;              /* j was parallel to i? */
        xj /= val ;
        yj /= val ;
        zj /= val ;
    }

    /* normalize k axis; if it is zero, make it the cross product i x j */

    val = sqrt( xk*xk + yk*yk + zk*zk ) ;
    if( val == 0.0 ) {
        xk = yi*zj-zi*yj;
        yk = zi*xj-zj*xi ;
        zk=xi*yj-yi*xj ;
    }
    else            {
        xk /= val ;
        yk /= val ;
        zk /= val ;
    }

    /* orthogonalize k to i */

    val = xi*xk + yi*yk + zi*zk ;    /* dot product between i and k */
    if( fabs(val) > 1.e-4 ) {
        xk -= val*xi ;
        yk -= val*yi ;
        zk -= val*zi ;
        val = sqrt( xk*xk + yk*yk + zk*zk ) ;
        if( val == 0.0 )
            return ;      /* bad */
        xk /= val ;
        yk /= val ;
        zk /= val ;
    }

    /* orthogonalize k to j */

    val = xj*xk + yj*yk + zj*zk ;    /* dot product between j and k */
    if( fabs(val) > 1.e-4 ) {
        xk -= val*xj ;
        yk -= val*yj ;
        zk -= val*zj ;
        val = sqrt( xk*xk + yk*yk + zk*zk ) ;
        if( val == 0.0 )
            return ;      /* bad */
        xk /= val ;
        yk /= val ;
        zk /= val ;
    }

    Q(0, 0) = xi ;
    Q(0, 1) = xj ;
    Q(0, 2) = xk ;
    Q(1, 0) = yi ;
    Q(1, 1) = yj ;
    Q(1, 2) = yk ;
    Q(2, 0) = zi ;
    Q(2, 1) = zj ;
    Q(2, 2) = zk ;

    /* at this point, Q is the rotation matrix from the (i,j,k) to (x,y,z) axes */

    detQ = det( Q ) ;
    if( detQ == 0.0 )
        return ; /* shouldn't happen unless user is a DUFIS */

    /* Build and test all possible +1/-1 coordinate permutation matrices P;
       then find the P such that the rotation matrix M=PQ is closest to the
       identity, in the sense of M having the smallest total rotation angle. */

    /* Despite the formidable looking 6 nested loops, there are
       only 3*3*3*2*2*2 = 216 passes, which will run very quickly. */

    vbest = -666.0f ;
    ibest=pbest=qbest=rbest=1 ;
    jbest=2 ;
    kbest=3 ;
    for( i=1 ; i <= 3 ; i++ ) {    /* i = column number to use for row #1 */
        for( j=1 ; j <= 3 ; j++ ) {   /* j = column number to use for row #2 */
            if( i == j )
                continue ;
            for( k=1 ; k <= 3 ; k++ ) { /* k = column number to use for row #3 */
                if( i == k || j == k )
                    continue ;
                P(0, 0) = P(0, 1) = P(0, 2) =
                                        P(1, 0) = P(1, 1) = P(1, 2) =
                                                      P(2, 0) = P(2, 1) = P(2, 2) = 0.0f ;
                for( p=-1 ; p <= 1 ; p+=2 ) {   /* p,q,r are -1 or +1      */
                    for( q=-1 ; q <= 1 ; q+=2 ) {  /* and go into rows #1,2,3 */
                        for( r=-1 ; r <= 1 ; r+=2 ) {
                            P(0, i-1) = p ;
                            P(1, j-1) = q ;
                            P(2, k-1) = r ;
                            detP = det(P) ;           /* sign of permutation */
                            if( detP * detQ <= 0.0 )
                                continue ;  /* doesn't match sign of Q */
                            M = P * Q ;

                            /* angle of M rotation = 2.0*acos(0.5*sqrt(1.0+trace(M)))       */
                            /* we want largest trace(M) == smallest angle == M nearest to I */

                            val = M(0, 0) + M(1, 1) + M(2, 2) ; /* trace */
                            if( val > vbest ) {
                                vbest = val ;
                                ibest = i ;
                                jbest = j ;
                                kbest = k ;
                                pbest = p ;
                                qbest = q ;
                                rbest = r ;
                            }
                        }
                    }
                }
            }
        }
    }

    /* At this point ibest is 1 or 2 or 3; pbest is -1 or +1; etc.

       The matrix P that corresponds is the best permutation approxiarma::mation
       to Q-inverse; that is, P (approxiarma::mately) takes (x,y,z) coordinates
       to the (i,j,k) axes.

       For example, the first row of P (which contains pbest in column ibest)
       determines the way the i axis points relative to the anatomical
       (x,y,z) axes.  If ibest is 2, then the i axis is along the y axis,
       which is direction P2A (if pbest > 0) or A2P (if pbest < 0).

       So, using ibest and pbest, we can assign the output code for
       the i axis.  Mutatis mutandis for the j and k axes, of course. */

    switch( ibest*pbest ) {
    case  1:
        i = NIFTI_L2R ;
        break ;
    case -1:
        i = NIFTI_R2L ;
        break ;
    case  2:
        i = NIFTI_P2A ;
        break ;
    case -2:
        i = NIFTI_A2P ;
        break ;
    case  3:
        i = NIFTI_I2S ;
        break ;
    case -3:
        i = NIFTI_S2I ;
        break ;
    }

    switch( jbest*qbest ) {
    case  1:
        j = NIFTI_L2R ;
        break ;
    case -1:
        j = NIFTI_R2L ;
        break ;
    case  2:
        j = NIFTI_P2A ;
        break ;
    case -2:
        j = NIFTI_A2P ;
        break ;
    case  3:
        j = NIFTI_I2S ;
        break ;
    case -3:
        j = NIFTI_S2I ;
        break ;
    }

    switch( kbest*rbest ) {
    case  1:
        k = NIFTI_L2R ;
        break ;
    case -1:
        k = NIFTI_R2L ;
        break ;
    case  2:
        k = NIFTI_P2A ;
        break ;
    case -2:
        k = NIFTI_A2P ;
        break ;
    case  3:
        k = NIFTI_I2S ;
        break ;
    case -3:
        k = NIFTI_S2I ;
        break ;
    }

    *icod = i ;
    *jcod = j ;
    *kcod = k ;
    return ;
}

char const *Utils::nifti_orientation_string( int ii )
{
    switch( ii ) {
    case NIFTI_L2R:
        return "Left-to-Right" ;
    case NIFTI_R2L:
        return "Right-to-Left" ;
    case NIFTI_P2A:
        return "Posterior-to-Anterior" ;
    case NIFTI_A2P:
        return "Anterior-to-Posterior" ;
    case NIFTI_I2S:
        return "Inferior-to-Superior" ;
    case NIFTI_S2I:
        return "Superior-to-Inferior" ;
    }
    return "Unknown" ;
}

ScalarType Utils::det(itk::Matrix<ScalarType> R)
{
    ScalarType detR = R(0, 0) * R(1, 1) * R(2, 2) + R(0, 1) * R(1, 2) * R(2, 0) + R(1, 0) * R(2, 1) * R(0, 2) - R(0, 2) * R(1, 1) * R(2, 0) - R(1, 0) * R(0, 1) * R(2, 2) - R(0, 0) * R(2, 1) * R(1, 2);
    return detR;
}

Utils::Segmentation_Type Utils::AmosSegmentationType(const DataNode::Pointer node)
{
    const DataNode* nodePtr = node.GetPointer();
    return AmosSegmentationType(nodePtr);
}

Utils::Segmentation_Type Utils::AmosSegmentationType(const DataNode* node)
{
    if(!node)
        return Non_Amos;
    QString segType;
    Image* image = dynamic_cast<Image*>(node->GetData());
    if(!image)
        return Non_Amos;
    if(!image->GetProperty("descrip"))
        return Non_Amos;

    segType = QString::fromStdString(image->GetProperty("descrip")->GetValueAsString());
    if(segType.contains("Amos_AC"))
        return Amos_AC;
    else if(segType.contains("Amos_BC"))
        return Amos_BC;
    else
        return Non_Amos;
}

void Utils::changeGeometry(BaseGeometry* geometry, int* icod, int* jcod, int* kcod, int* reorderI, int* reorderJ, int* reorderK)
{
    AffineTransform3D* transform = geometry->GetIndexToWorldTransform();
    itk::Matrix<ScalarType> qform = transform->GetMatrix();
    Utils::nifti_mat_to_orientation(qform, icod, jcod, kcod);
    cout << qform << endl;

    int i_cod, j_cod, k_cod;

    Utils::nifti_mat_to_orientation(qform, &i_cod, &j_cod, &k_cod);
    cout << Utils::nifti_orientation_string(i_cod) << " " << Utils::nifti_orientation_string(j_cod) << " " << Utils::nifti_orientation_string(k_cod) << endl;
    if(logSender)
        logSender->logText(QString::fromLatin1(Utils::nifti_orientation_string(i_cod)) + " " + QString::fromLatin1(Utils::nifti_orientation_string(j_cod)) + " " + QString::fromLatin1(Utils::nifti_orientation_string(k_cod)));

    *reorderI = (*icod + 1) % 2;
    *reorderJ = (*jcod + 1) % 2;
    *reorderK = (*kcod + 1) % 2;
    int iq = (*icod + 1) / 2;
    int jq = (*jcod + 1) / 2;
    int kq = (*kcod + 1) / 2;

    Col<int> indexes;
    indexes << iq << jq << kq;
    uvec uveci = find(indexes == 1);
    uvec uvecj = find(indexes == 2);
    uvec uveck = find(indexes == 3);
    *icod = uveci(0) + 1;
    *jcod = uvecj(0) + 1;
    *kcod = uveck(0) + 1;

    Col<int> reorders;
    reorders << *reorderI << *reorderJ << *reorderK;
    cout << uveci(0) << " " << uvecj(0) << " " << uveck(0) << endl;
    *reorderI = reorders(uveci(0));
//     *reorderI = *reorderI == 0 ? 1 : 0; // for LPS not RAS in MITK
    *reorderJ = reorders(uvecj(0));
//     *reorderJ = *reorderJ == 0 ? 1 : 0; // uncommenting eyes looking down
    *reorderK = reorders(uveck(0));
//     *reorderK = *reorderK == 0 ? 1 : 0;

    cout << *reorderI << " " << *reorderJ << " " << *reorderK << endl;

//     int i = iq - 1;
//     int j = jq - 1;
//     int k = kq - 1;

    int i = uveci(0);
    int j = uvecj(0);
    int k = uveck(0);
    cout << i << " " << j << " " << k << endl;

    itk::Matrix<ScalarType> reorderMat;
    reorderMat.Fill(0);
//     reorderMat(0, i) = *reorderI == 0 ? 1 : -1;
//     reorderMat(1, j) = *reorderJ == 0 ? 1 : -1;
//     reorderMat(2, k) = *reorderK == 0 ? 1 : -1;
    reorderMat(i, 0) = *reorderI == 0 ? 1 : -1;
    reorderMat(j, 1) = *reorderJ == 0 ? 1 : -1;
    reorderMat(k, 2) = *reorderK == 0 ? 1 : -1;
    cout << reorderMat << endl;
    qform = qform * reorderMat;
    transform->SetMatrix(qform);

    Point3D origin = geometry->GetOrigin();
    ScalarType originArray[3] = {0.0, 0.0, 0.0};
    origin.ToArray(originArray);
//     cout << "Origin1 " << origin[0] << " " << origin[1] << " " << origin[2] << endl;
    ScalarType newArray[3] = {0.0, 0.0, 0.0};
    /*    newArray[i] = originArray[0] * reorderMat(0, i);
        newArray[j] = originArray[1] * reorderMat(1, j);
        newArray[k] = originArray[2] * reorderMat(2, k);*/
    newArray[0] = originArray[i] * reorderMat(i, 0);
    newArray[1] = originArray[j] * reorderMat(j, 1);
    newArray[2] = originArray[k] * reorderMat(k, 2);

    origin.FillPoint(newArray);
    geometry->SetOrigin(origin);

    BoundingBox::BoundsArrayType oldBounds = geometry->GetBounds();
    cout << "oldBounds " << oldBounds[1] << " " << oldBounds[3] << " " << oldBounds[5] << endl;
    mitk::Vector3D startBounds;
    /*    startBounds[i] = oldBounds[0];
        startBounds[j] = oldBounds[2];
        startBounds[k] = oldBounds[4];*/
//     startBounds[0] = oldBounds[2*i];
//     startBounds[1] = oldBounds[2*j];
//     startBounds[2] = oldBounds[2*k];
    startBounds[iq - 1] = oldBounds[0];
    startBounds[jq - 1] = oldBounds[2];
    startBounds[kq - 1] = oldBounds[4];
    mitk::Vector3D endBounds;
//     endBounds[i] = oldBounds[1];
//     endBounds[j] = oldBounds[3];
//     endBounds[k] = oldBounds[5];
    /*    endBounds[i] = oldBounds[2*i + 1];
        endBounds[j] = oldBounds[2*j + 1];
        endBounds[k] = oldBounds[2*k + 1];*/
    endBounds[iq - 1] = oldBounds[1];
    endBounds[jq - 1] = oldBounds[3];
    endBounds[kq - 1] = oldBounds[5];

    BoundingBox::BoundsArrayType newBounds;
    newBounds.Fill(0);
    newBounds[0] = startBounds[0];
    newBounds[2] = startBounds[1];
    newBounds[4] = startBounds[2];
    newBounds[1] = endBounds[0];
    newBounds[3] = endBounds[1];
    newBounds[5] = endBounds[2];
    cout << "newBounds " << newBounds[1] << " " << newBounds[3] << " " << newBounds[5] << endl;
    geometry->SetBounds(newBounds);

    SlicedGeometry3D* geoSlc3D = (SlicedGeometry3D*) geometry;
    if(geoSlc3D) {
        // 	geoSlc3D->SetPlaneGeometry(planeGeo, 0);
        // 	geoSlc3D->InitializeEvenlySpaced(planeGeo, FL3Dcube.n_slices);
        mitk::Vector3D spacing = geometry->GetSpacing();
        PlaneGeometry::Pointer planeGeo = geoSlc3D->GetPlaneGeometry(0)->Clone();
        if(planeGeo) {
            cout << "has planeGeo" << endl;
            planeGeo->SetImageGeometry(true);
            planeGeo->GetIndexToWorldTransform()->SetMatrix(qform);
//             planeGeo->SetReferenceGeometry(geometry);
//             planeGeo->SetIndexToWorldTransform(transform);
//             planeGeo->SetOrigin(origin);
//             planeGeo->SetBounds(newBounds);
//             planeGeo->SetSpacing(spacing);

//             planeGeo->Print(cout, 4);

//             geoSlc3D->InitializeEvenlySpaced(planeGeo, newBounds[5] - newBounds[4]);
            const mitk::Vector3D dirVector =  geoSlc3D->GetDirectionVector();
            mitk::Vector3D newDirVector;
            newDirVector[0] = dirVector[i] * reorderMat(i, 0);
            newDirVector[1] = dirVector[j] * reorderMat(j, 1);
            newDirVector[2] = dirVector[k] * reorderMat(k, 2);
            geoSlc3D->InitializeSlicedGeometry(newBounds[5] - newBounds[4]);
            geoSlc3D->SetIndexToWorldTransform(planeGeo->GetIndexToWorldTransform());
            geoSlc3D->SetOrigin(origin);
            geoSlc3D->SetDirectionVector(newDirVector);
            geoSlc3D->SetBounds(newBounds);
            geoSlc3D->SetSpacing(spacing);
            geoSlc3D->SetImageGeometry(true);
            for(int i = 0; i < newBounds[5] - newBounds[4]; i++)
                geoSlc3D->SetPlaneGeometry(planeGeo, i);
//             geoSlc3D->InitializeEvenlySpaced(planeGeo, newBounds[5] - newBounds[4]);

        }

//         geoSlc3D->SetIndexToWorldTransform(planeGeo->GetIndexToWorldTransform());
//         geoSlc3D->SetOrigin(origin);
//         const mitk::Vector3D dirVector =  geoSlc3D->GetDirectionVector();
//         mitk::Vector3D newDirVector;
//         newDirVector[0] = dirVector[i] * reorderMat(i, 0);
//         newDirVector[1] = dirVector[j] * reorderMat(j, 1);
//         newDirVector[2] = dirVector[k] * reorderMat(k, 2);
//         geoSlc3D->SetDirectionVector(newDirVector);
//         cout << dirVector << endl;
//         cout << newDirVector << endl;
//         planeGeo->Print(cout, 4);

//         geoSlc3D->Print(cout, 4);
    }

    Utils::nifti_mat_to_orientation(qform, &i_cod, &j_cod, &k_cod);
    cout << Utils::nifti_orientation_string(i_cod) << " " << Utils::nifti_orientation_string(j_cod) << " " << Utils::nifti_orientation_string(k_cod) << endl;
    if(logSender)
        logSender->logText(QString::fromLatin1(Utils::nifti_orientation_string(i_cod)) + " " + QString::fromLatin1(Utils::nifti_orientation_string(j_cod)) + " " + QString::fromLatin1(Utils::nifti_orientation_string(k_cod)));

    cout << qform << endl;
}

DataOrientation::OrientationData Utils::invertOData(DataOrientation::OrientationData oData)
{
    /*    Col<int> indexes;
        indexes << oData.icod << oData.jcod << oData.kcod;
        uvec uveci = find(indexes == 1);
        uvec uvecj = find(indexes == 2);
        uvec uveck = find(indexes == 3);
        int i = uveci(0);
        int j = uvecj(0);
        int k = uveck(0); */

    arma::Mat<char> reorderMat(3, 3);
    reorderMat.zeros();
    reorderMat(oData.icod - 1, 0) = oData.reorderI == 0 ? 1 : -1;
    reorderMat(oData.jcod - 1, 1) = oData.reorderJ == 0 ? 1 : -1;
    reorderMat(oData.kcod - 1, 2) = oData.reorderK == 0 ? 1 : -1;
    cout << reorderMat << endl;

    arma::Mat<char> invReorderMat(3, 3);

    invReorderMat = reorderMat.t();
    cout << invReorderMat << endl;

    DataOrientation::OrientationData invOData;
//     uveci = find(invReorderMat.row(0));
//     uvecj = find(invReorderMat.row(1));
//     uveck = find(invReorderMat.row(2));
    uvec uveci = find(invReorderMat.col(0));
    uvec uvecj = find(invReorderMat.col(1));
    uvec uveck = find(invReorderMat.col(2));
    invOData.icod = uveci(0);
    invOData.jcod = uvecj(0);
    invOData.kcod = uveck(0);
//     invOData.reorderI = invReorderMat(0, invOData.icod) == 1 ? 0 : 1;
//     invOData.reorderJ = invReorderMat(1, invOData.jcod) == 1 ? 0 : 1;
//     invOData.reorderK = invReorderMat(2, invOData.kcod) == 1 ? 0 : 1;
    invOData.reorderI = invReorderMat(invOData.icod, 0) == 1 ? 0 : 1;
    invOData.reorderJ = invReorderMat(invOData.jcod, 1) == 1 ? 0 : 1;
    invOData.reorderK = invReorderMat(invOData.kcod, 2) == 1 ? 0 : 1;
    invOData.icod++;
    invOData.jcod++;
    invOData.kcod++;

    cout << oData.icod << oData.jcod << oData.kcod << oData.reorderI << oData.reorderJ << oData.reorderK << endl;
    cout << invOData.icod << invOData.jcod << invOData.kcod << invOData.reorderI << invOData.reorderJ << invOData.reorderK << endl;

    return invOData;

}

void Utils::revertGeometry(BaseGeometry* geometry, int iq, int jq, int kq, int reorderI, int reorderJ, int reorderK)
{
    AffineTransform3D* transform = geometry->GetIndexToWorldTransform();
    itk::Matrix<ScalarType> qform = transform->GetMatrix();
    /*    Utils::nifti_mat_to_orientation(qform, icod, jcod, kcod);*/

    /*    *reorderI = (*icod + 1) % 2;
        *reorderJ = (*jcod + 1) % 2;
        *reorderK = (*kcod + 1) % 2*/;
//     int iq = (*icod + 1) / 2;
//     int jq = (*jcod + 1) / 2;
//     int kq = (*kcod + 1) / 2;

    /*    Col<int> indexes;
        indexes << iq << jq << kq;
        uvec uveci = find(indexes == 1);
        uvec uvecj = find(indexes == 2);
        uvec uveck = find(indexes == 3);
        *icod = uveci(0) + 1;
        *jcod = uvecj(0) + 1;
        *kcod = uveck(0) + 1;

        Col<int> reorders;
        reorders << *reorderI << *reorderJ << *reorderK;
        *reorderI = reorders(uveci(0));
        *reorderI = *reorderI == 0 ? 1 : 0; // for LAS not RAS in MITK
        *reorderJ = reorders(uvecj(0));
        *reorderK = reorders(uveck(0));*/

    int i = iq - 1;
    int j = jq - 1;
    int k = kq - 1;
    cout << i << " " << j << " " << k << endl;
    cout << reorderI << " " << reorderJ << " " << reorderK << endl;

    itk::Matrix<ScalarType> reorderMat;
    reorderMat.Fill(0);

    Col<int> indexes;
    indexes << reorderI << reorderJ << reorderK;

    // traspose equal inverse
//     reorderMat(0, i) = reorderI == 0 ? 1 : -1;
//     reorderMat(1, j) = reorderJ == 0 ? 1 : -1;
//     reorderMat(2, k) = reorderK == 0 ? 1 : -1;
    /*    reorderMat(i, 0) = indexes(i) == 0 ? 1 : -1;
        reorderMat(j, 1) = indexes(j) == 0 ? 1 : -1;
        reorderMat(k, 2) = indexes(k) == 0 ? 1 : -1;*/
    reorderMat(i, 0) = reorderI == 0 ? 1 : -1;
    reorderMat(j, 1) = reorderJ == 0 ? 1 : -1;
    reorderMat(k, 2) = reorderK == 0 ? 1 : -1;

    cout << qform << endl;
    cout << reorderMat << endl;

    qform = qform * reorderMat;
    transform->SetMatrix(qform);
    cout << qform << endl;

    Point3D origin = geometry->GetOrigin();
    ScalarType originArray[3] = {0.0, 0.0, 0.0};
    origin.ToArray(originArray);
//     cout << "Origin1 " << origin[0] << " " << origin[1] << " " << origin[2] << endl;
    ScalarType newArray[3] = {0.0, 0.0, 0.0};
    /*    newArray[i] = originArray[0] * reorderMat(0, i);
        newArray[j] = originArray[1] * reorderMat(1, j);
        newArray[k] = originArray[2] * reorderMat(2, k);*/
    newArray[0] = originArray[i] * reorderMat(i, 0);
    newArray[1] = originArray[j] * reorderMat(j, 1);
    newArray[2] = originArray[k] * reorderMat(k, 2);

    origin.FillPoint(newArray);
    geometry->SetOrigin(origin);

    BoundingBox::BoundsArrayType oldBounds = geometry->GetBounds();
    cout << "oldBounds " << oldBounds[1] << " " << oldBounds[3] << " " << oldBounds[5] << endl;
    mitk::Vector3D startBounds;
//     startBounds[i] = oldBounds[0];
//     startBounds[j] = oldBounds[2];
//     startBounds[k] = oldBounds[4];
    startBounds[0] = oldBounds[2*i];
    startBounds[1] = oldBounds[2*j];
    startBounds[2] = oldBounds[2*k];
    mitk::Vector3D endBounds;
//     endBounds[i] = oldBounds[1];
//     endBounds[j] = oldBounds[3];
//     endBounds[k] = oldBounds[5];
    endBounds[0] = oldBounds[2*i + 1];
    endBounds[1] = oldBounds[2*j + 1];
    endBounds[2] = oldBounds[2*k + 1];

    BoundingBox::BoundsArrayType newBounds;
    newBounds.Fill(0);
    newBounds[0] = startBounds[0];
    newBounds[2] = startBounds[1];
    newBounds[4] = startBounds[2];
    newBounds[1] = endBounds[0];
    newBounds[3] = endBounds[1];
    newBounds[5] = endBounds[2];
    cout << "newBounds " << newBounds[1] << " " << newBounds[3] << " " << newBounds[5] << endl;
    geometry->SetBounds(newBounds);

    SlicedGeometry3D* geoSlc3D = (SlicedGeometry3D*) geometry;
    if(geoSlc3D) {
// 	geoSlc3D->SetPlaneGeometry(planeGeo, 0);
// 	geoSlc3D->InitializeEvenlySpaced(planeGeo, FL3Dcube.n_slices);
        PlaneGeometry* planeGeo = geoSlc3D->GetPlaneGeometry(0);
        if(planeGeo) {
            cout << "has planeGeo" << endl;
            planeGeo->SetIndexToWorldTransform(transform);
            planeGeo->SetOrigin(origin);
            planeGeo->SetBounds(newBounds);
            mitk::Vector3D spacing = geometry->GetSpacing();
            planeGeo->SetSpacing(spacing);
        }
        const mitk::Vector3D dirVector =  geoSlc3D->GetDirectionVector();
        mitk::Vector3D newDirVector;
// 	newDirVector[i] = dirVector[0] * reorderMat(0, i);
// 	newDirVector[j] = dirVector[1] * reorderMat(1, j);
// 	newDirVector[k] = dirVector[2] * reorderMat(2, k);
        newDirVector[0] = dirVector[i] * reorderMat(i, 0);
        newDirVector[1] = dirVector[j] * reorderMat(j, 1);
        newDirVector[2] = dirVector[k] * reorderMat(k, 2);
        geoSlc3D->SetDirectionVector(newDirVector);
    }

    int i_cod, j_cod, k_cod;

    Utils::nifti_mat_to_orientation(qform, &i_cod, &j_cod, &k_cod);
    cout << Utils::nifti_orientation_string(i_cod) << " " << Utils::nifti_orientation_string(j_cod) << " " << Utils::nifti_orientation_string(k_cod) << endl;
}


QList<int> Utils::getRowsForItems(QList<QTableWidgetItem *> items)
{
    QList<int> rows;
    int cnt = items.size();
    for(int i = 0; i < cnt; i++) {
        if(!rows.contains(items.at(i)->row()))
            rows.append(items.at(i)->row());
    }
    return rows;
}

/**
 * Returns the total size of phyiscal memory in bytes
 */
size_t Utils::getTotalSizeOfFreeRam()
{
#if _MSC_VER || __MINGW32__
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return (size_t) statex.ullAvailPhys;
#elif defined(__APPLE__)
    int64_t free_memory = 0;
    int rc;
    u_int page_size;
    struct vmtotal vmt;
    size_t vmt_size, uint_size; 

    vmt_size = sizeof(vmt);
    uint_size = sizeof(page_size);
    
    rc = sysctlbyname("vm.vmtotal", &vmt, &vmt_size, NULL, 0);
    if (rc < 0){
//        perror("sysctlbyname");
       return free_memory;
    }
    
    rc = sysctlbyname("vm.stats.vm.v_page_size", &page_size, &uint_size, NULL, 0);
    if (rc >= 0){
//        perror("sysctlbyname");
       //physical_memory = vmt.t_avm * (u_int64_t)page_size);
       free_memory = vmt.t_free * (u_int64_t)page_size;
    }    
    
    return free_memory;
#else
    struct sysinfo info;
    if ( ! sysinfo( &info ) )
        return info.freeram * info.mem_unit;
    else
        return 0;
#endif
}
