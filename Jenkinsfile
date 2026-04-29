pipeline {
	agent any
	options {
	    skipDefaultCheckout(true)
	}
	stages {
		stage('Clone'){
			steps {
			    cleanWs()
			    // checkout scm
			    git (
			        url: "https://github.com/rertzer/webserv.git",
			        branch: "master"
			    )
				sh 'mkdir log'
				sh 'echo "build: ${BUILD_TAG}"'
			}
		}
		stage('Build'){
			agent {
				docker{
					image 'silkeh/clang'
					reuseNode true
				}
			}
			steps{
				sh 'make all | tee log/build.log'
				sh 'make clean'
				sh 'make confpath'
			}
		}
		stage('Test Functional'){
			agent {
				docker{
					image 'python_agent'
					args '--sysctl net.ipv4.ip_unprivileged_port_start=1024'
					reuseNode true
				}
			}
				steps{
				    sh 'cd tests/ && ./webtest.py | tee ../log/func_tests.log'
				}
			}
	}
	post {
		failure {
			emailext(
				subject: "Build #${env.BUILD_NUMBER} - ${currentBuild.currentResult} (${env.JOB_NAME})",
				body: """\
			Hello,

			Your build has completed.

			Job: ${env.JOB_NAME}
			Build Number: ${env.BUILD_NUMBER}
			Status: ${currentBuild.currentResult}

			You can find more details here:
			${env.BUILD_URL}

			Regards,
			Webkins
			""",
				to: '',
				recipientProviders: [
					[$class: 'RequesterRecipientProvider']
				],
				attachmentsPattern: 'log/*.log',
				mimeType: 'text/plain'
			)
		}
	}
}
