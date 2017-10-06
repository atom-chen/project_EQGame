using Common;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;
using Model;

namespace TestCommon
{
    
    
    /// <summary>
    ///This is a test class for XmlTest and is intended
    ///to contain all XmlTest Unit Tests
    ///</summary>
    [TestClass()]
    public class XmlTest
    {


        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        // 
        //You can use the following additional attributes as you write your tests:
        //
        //Use ClassInitialize to run code before running the first test in the class
        //[ClassInitialize()]
        //public static void MyClassInitialize(TestContext testContext)
        //{
        //}
        //
        //Use ClassCleanup to run code after all tests in a class have run
        //[ClassCleanup()]
        //public static void MyClassCleanup()
        //{
        //}
        //
        //Use TestInitialize to run code before running each test
        //[TestInitialize()]
        //public void MyTestInitialize()
        //{
        //}
        //
        //Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //public void MyTestCleanup()
        //{
        //}
        //
        #endregion

        [TestMethod()]
        public void DeserializeTest()
        {
            XmlManager target = new XmlManager();
            string path = @"E:\work\eq\server\tools\script_editor\TestCommon\bin\Debug\map\common_scene.xml";
            SceneInfo actual;
            actual = target.Deserialize<SceneInfo>(path);
            Assert.AreEqual(1, actual.common_scene[0].id);
        }
    }
}
