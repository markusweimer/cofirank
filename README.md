CoFiRank Release 0.1
====================

This is CoFiRank -- Collaborative Filtering for Ranking -- code and Copyright
(c) 2009 by Markus Weimer, Alexandros Karatzoglou, Alex Smola. CoFiRank
includes code contributions by Quoc Le, and Choon Hui Teo.

CoFiRank is an open source library for collaborative filtering and matrix
factorization licensed under the Mozilla Public License (MPL). The homepage is
at:

  http://www.cofirank.org



Compilation
-----------

CoFiRank requires the [boost libraries][boost] to be installed
on your system. By default CoFiRank also looks in the ${CoFiRank}/libs
directory for boost, if a system-wide installation is not possible at your
site.

CoFiRank can be compiled by either importing the code as a project into
NetBeans (we used version 6.5) or at the command line using the make command:

    make -f CofiRank-Makefile.mk CONF=Deploy

to compile the code without Debug information, while 

    make -f CofiRank-Makefile.mk CONF=Debug

includes Debug information. The binaries are then saved in
`dist/cofirank-deploy` or `dist/cofirank-debug` respectively.

Running:
--------
The code can be run on the command line as follows:

    ./dist/cofirank-deploy config/default.cfg

note that the first command line argument has to be a config file.

Output 
-------

Output is saved in a directory specified in the config file with the option
`cofi.outfolder`. Output includes a file where the results of the evaluation
are stored per iteration `result.cvs`, a file with runtime information
`clog.txt`, a file containing the configuration options used
`effective-configuration.cfg`, and optionally files containing the model
`U.lsvm`, `M.lsvm` and the predicted output `F.lsvm`.


File Format for the Input Matrix
--------------------------------

We use the format from [libsvm][libsvm]. Each row of the input data
corresponds to one row in the input matrix or one user in the cofi case. In
each row col:value pairs encode the entries of this sparse matrix, where col
starts with 1 and is expected to be sorted increasingly per row. Example:

    1:5 39:2 65:1
    12:3 26:1 77:5

This encodes two users, where user 0 has seen the movies 1,39 and 65. User 1
has seen movie 12, 26, 77



Configuration:
--------------

This is a short description of each configuration option in CoFiRank, for
details on the individual configuration options please refer to the published
papers.

<code>
string   cofi.outfolder                          PATH // The folder where all output will be stored
string   cofibmrm.DtestFile                      FILENAMES   // Path to the test data
string   cofibmrm.DtrainFile                     FILENAMES   // Path to the train data
int      cofi.trainfile.size1                    Positive integer // Number of rows of the train file. If not given, it will be computed.
int      cofi.trainfile.size2                    Positive integer  //    Number of cols of the train file. If not given, it will be computed.

int      cofi.storeU                             0/1      // whether or not U, M and F shall be stored at the end
int      cofi.storeM                             0/1      // 0 -> FALSE
int      cofi.storeF                             0/1      // 1 -> TRUE

double   cofi.minProgress                        0.1 // Terminate when overall objective[t] - objective[t-1]/objective[t-1] < minProgress
int      cofi.minIterations                      3   // Min. number of CoFi iterations over U and M
int      cofi.maxIterations                      30  // Max number of CoFi iterations over U and M

int      cofi.dimW    10                     // a positive integer    The number of features to learn
int      cofi.useAdaptiveRegularization          0/1     // Whether or not we want to use adaptive regularization
double   cofi.adaptiveRegularization.uExponent   1.0  // pow((#movies/max#movies),uExponent) scaling for each user-regularization value
double   cofi.adaptiveRegularization.wExponent   1.0  // pow((#users/max#users),wExponent) scaling for each movie-regularization value 
int      cofi.useMovieOffset                     0/1           //    Enables or disables the item offset.
int      cofi.useUserOffset                      0/1           //    Enables or disables the item offset.
int      cofi.useGraphKernel                     0/1           //    whether or not top use the GraphKernel 

string   cofi.loss                               REGRESSION/ NDCG / ORDINAL   // The loss to optimize for
string   cofibmrm.evaluation                     WEAK, STRONG  //    Evaluation in weak or strong mode
double   cofi.userphase.lambda                   10.0          //    Userphase  regularization parameter lambda
double   cofi.moviephase.lambda                  10.0          //    Moviephase regularization parameter lambda

int      cofi.eval.binary                        0/1  //    Enable disable binary classification evaluation
int      cofi.eval.mse                           0/1  //    Enable disable the RMSE evaluation
int      cofi.eval.ndcg                          0/1  //    Enable disable NDCG evaluation
int      cofi.eval.ndcg.k                        10   //        a positive integer, the truncation value in NDCG@k
int      cofi.eval.brmse                         0/1  //    Enable / disable binary rmse

double   bmrm.gammaTol                           0.01   // Terminate BMRM when objective[t] - objective[t-1]/objective[t-1] < gammaTol
double   bmrm.epsilonTol                         -1.0   // Terminate BMRM when objective[t] - objective[t-1] < minProgress (negative values turns this off)
int      bmrm.maxIter                            4000   // Maximum number of BMRM iterations

int      loss.ndcg.trainK                        10   // Truncation value for NDCG loss
double   loss.ndcg.c_exponent                    -0.25 // c exponent for NDCG loss (see nips paper for details)
</code>




[boost]: http://www.boost.org
[libsvm]: http://www.csie.ntu.edu.tw/~cjlin/libsvm/